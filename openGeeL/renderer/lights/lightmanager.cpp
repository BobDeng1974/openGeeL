#define GLEW_STATIC
#include <glew.h>
#include "pointlight.h"
#include "directionallight.h"
#include "spotlight.h"
#include "../cameras/camera.h"
#include "../framebuffer/gbuffer.h"
#include "../transformation/transform.h"
#include "../shader/rendershader.h"
#include "../shader/sceneshader.h"
#include "../shadowmapping/shadowmap.h"
#include "../shadowmapping/simpleshadowmap.h"
#include "../shadowmapping/cascadedmap.h"
#include "../cubemapping/reflectionprobe.h"
#include "../cubemapping/iblmap.h"
#include "../framebuffer/cubebuffer.h"
#include "../voxelization/voxelstructure.h"
#include "../scene.h"
#include "light.h"
#include "lightbinding.h"
#include "lightmanager.h"

using namespace std;
using namespace glm;

namespace geeL {

	LightManager::LightManager() : ambient(ambient), voxelStructure(nullptr),
		dlShader(new RenderShader("renderer/shadowmapping/shadowmapping.vert", "renderer/shaders/empty.frag")),
		plShader(new RenderShader("renderer/shaders/empty.vert", "renderer/shadowmapping/shadowmapping.geom", 
				"renderer/shadowmapping/shadowmapping.frag")) {}


	LightManager::~LightManager() {
		delete dlShader;
		delete plShader;

		for (auto it = pointLights.begin(); it != pointLights.end(); it++) {
			Light* light = (*it).light;
			light->remove();

			delete light;
		}
			
		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			Light* light = (*it).light;
			light->remove();

			delete light;
		}

		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			Light* light = (*it).light;
			light->remove();

			delete light;
		}

		for (auto it = reflectionProbes.begin(); it != reflectionProbes.end(); it++)
			delete *it;
	}

	DirectionalLight& LightManager::addDirectionalLight(const SceneCamera& camera, Transform& transform, 
		vec3 diffuse, const ShadowMapConfiguration& config) {
		
		DirectionalLight* light = new DirectionalLight(transform, diffuse);
		
		size_t index = dirLights.size();
		DLightBinding d = DLightBinding(light, index, dlName);
		dirLights.push_back(std::move(d));

		if (config.useShadowMap()) {
			//SimpleDirectionalLightMap* map = new SimpleDirectionalLightMap(*light, config.shadowBias, 100.f);
			CascadedDirectionalShadowMap* map = new CascadedDirectionalShadowMap(*light, camera, config.shadowBias, 1024, 1024);
			light->setShadowMap(*map);
		}

		onAdd(light, dirLights.back());
		return *light;
	}

	
	PointLight& LightManager::addPointLight(Transform& transform, vec3 diffuse, const ShadowMapConfiguration& config) {
		PointLight* light = new PointLight(transform, diffuse);
		
		size_t index = pointLights.size();
		PLightBinding p = PLightBinding(light, index, plName);
		pointLights.push_back(std::move(p));

		if (config.useShadowMap()) {
			SimplePointLightMap* map = new SimplePointLightMap(*light, config);
			light->setShadowMap(*map);
		}
		
		onAdd(light, pointLights.back());
		return *light;
	}

	SpotLight& LightManager::addSpotlight(Transform& transform, vec3 diffuse,
		float angle, float outerAngle, const ShadowMapConfiguration& config) {

		SpotLight* light = new SpotLight(transform, diffuse, angle, outerAngle);

		size_t index = spotLights.size();
		SLightBinding s = SLightBinding(light, index, slName);
		spotLights.push_back(std::move(s));

		if (config.useShadowMap()) {
			SimpleSpotLightMap* map = new SimpleSpotLightMap(*light, config);
			light->setShadowMap(*map);
		}
		
		onAdd(light, spotLights.back());
		return *light;
	}

	void LightManager::removeLight(DirectionalLight& light) {
		//Find corresponding light binding
		DLightBinding* binding = getBinding<DLightBinding, DirectionalLight,
			std::list<DLightBinding>>(light, dirLights);

		if (binding != nullptr) {
			dirLights.remove(*binding);
			reindexLights<std::list<DLightBinding>>(dirLights);

			onRemove(&light, *binding);
			delete &light;
		}
	}

	void LightManager::removeLight(PointLight& light) {
		//Find corresponding light binding
		PLightBinding* binding = getBinding<PLightBinding, PointLight,
			std::list<PLightBinding>>(light, pointLights);

		if (binding != nullptr) {
			pointLights.remove(*binding);
			reindexLights<std::list<PLightBinding>>(pointLights);

			onRemove(&light, *binding);
			delete &light;
		}
	}

	void LightManager::removeLight(SpotLight& light) {
		//Find corresponding light binding
		SLightBinding* binding = getBinding<SLightBinding, SpotLight,
			std::list<SLightBinding>>(light, spotLights);

		if (binding != nullptr) {
			spotLights.remove(*binding);
			reindexLights<std::list<SLightBinding>>(spotLights);

			onRemove(&light, *binding);
			delete &light;
		}
	}

	DynamicIBLMap& LightManager::addReflectionProbe(const DynamicIBLMap& probe) {
		DynamicIBLMap* map = new DynamicIBLMap(probe);
		reflectionProbes.push_back(map);

		return *map;
	}

	IBLMap& LightManager::addReflectionProbe(const IBLMap& probe) {
		IBLMap* map = new IBLMap(probe);
		reflectionProbes.push_back(map);

		return *map;
	}

	void LightManager::removeReflectionProbe(DynamicCubeMap& probe) {
		//TODO: implement this
		reflectionProbes.remove(&probe);
	}

	void LightManager::addReflectionProbes(RenderShader& shader) const {
		shader.use();

		for (auto it = reflectionProbes.begin(); it != reflectionProbes.end(); it++) {
			DynamicCubeMap& probe = **it;
			probe.add(shader, "skybox.");
		}
	}

	void LightManager::bindReflectionProbes(const Camera& camera, const RenderShader& shader, ShaderTransformSpace space) const {
		shader.use();

		unsigned int rpCount = 0;
		for (auto it = reflectionProbes.begin(); it != reflectionProbes.end(); it++) {
			DynamicCubeMap& probe = **it;

			probe.bind(camera, shader, "skybox.", space);
			rpCount++;
		}

		//shader.setInteger(rpCountName, rpCount);
	}


	void LightManager::drawReflectionProbes() const {
		for (auto it = reflectionProbes.begin(); it != reflectionProbes.end(); it++) {
			DynamicCubeMap& probe = **it;
			probe.update();
		}
	}

	void LightManager::bind(const Camera& camera, const RenderShader& shader, ShaderTransformSpace space) const {
		shader.use();

		unsigned int plCount = 0;
		unsigned int dlCount = 0;
		unsigned int slCount = 0;

		for (auto it = pointLights.begin(); it != pointLights.end(); it++) {
			const PLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive()) {
				light.bind(camera, shader, binding.getName(plCount), space);
				plCount++;
			}
		}

		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			const DLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive()) {
				light.bind(camera, shader, binding.getName(dlCount), space);
				dlCount++;
			}
		}
			
		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			const SLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive()) {
				light.bind(camera, shader, binding.getName(slCount), space);
				slCount++;
			}
		}
		
		shader.setInteger(plCountName, plCount);
		shader.setInteger(dlCountName, dlCount);
		shader.setInteger(slCountName, slCount);
	}

	void LightManager::bind(const Camera& camera, const SceneShader& shader) const {
		bind(camera, shader, shader.getSpace());
	}


	void LightManager::bindShadowmap(RenderShader& shader, PointLight& light) const {
		//Find corresponding light binding
		const PLightBinding* binding = getBinding<PLightBinding, PointLight,
			std::list<PLightBinding>>(light, pointLights);

		//Bind shadowmap
		if(binding != nullptr)
			light.addShadowmap(shader, binding->getName() + "shadowMap");
	}

	void LightManager::bindShadowmap(RenderShader& shader, SpotLight& light) const {
		//Find corresponding light binding
		const SLightBinding* binding = getBinding<SLightBinding, SpotLight, 
			std::list<SLightBinding>>(light, spotLights);

		//Bind shadowmap
		if (binding != nullptr)
			light.addShadowmap(shader, binding->getName() + "shadowMap");
	}

	
	void LightManager::bindShadowmap(RenderShader& shader, DirectionalLight& light) const {
		//Find corresponding light binding
		const DLightBinding* binding = getBinding<DLightBinding, DirectionalLight,
			std::list<DLightBinding>>(light, dirLights);

		//Bind shadowmap
		if (binding != nullptr)
			light.addShadowmap(shader, binding->getName() + "shadowMap");
	}

	void LightManager::bindShadowMaps(RenderShader& shader) const {
		shader.use();

		for (auto it = pointLights.begin(); it != pointLights.end(); it++) {
			const PLightBinding& binding = *it;
			Light& light = *binding.light;
			light.addShadowmap(shader, binding.getName() + "shadowMap");
		}
		
		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			const DLightBinding& binding = *it;
			Light& light = *binding.light;
			light.addShadowmap(shader, binding.getName() + "shadowMap");
		}

		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			const SLightBinding& binding = *it;
			SpotLight& light = *binding.light;

			light.addShadowmap(shader, binding.getName() + "shadowMap");
			light.addLightCookie(shader, binding.getName() + "cookie");
		}
	}

	void LightManager::draw(const RenderScene& scene, const SceneCamera* const camera) {
		drawShadowmaps(scene, camera);
		drawVoxelStructure();
	}


	void LightManager::drawShadowmaps(const RenderScene& scene, const SceneCamera* const camera) const {
		glCullFace(GL_BACK);

		for(auto it = pointLights.begin(); it != pointLights.end(); it++) {
			const PLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmap(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *plShader);
		}

		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			const DLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmap(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *dlShader);
		}

		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			const SLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmap(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *dlShader);
		}	

		glCullFace(GL_FRONT);
	}

	
	void LightManager::drawShadowmapsForced(const RenderScene& scene, const SceneCamera* const camera) const {
		glCullFace(GL_BACK);

		for (auto it = pointLights.begin(); it != pointLights.end(); it++) {
			const PLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmapForced(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *plShader);
		}

		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			const DLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmapForced(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *dlShader);
		}

		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			const SLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmapForced(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *dlShader);
		}

		glCullFace(GL_FRONT);
	}


	void LightManager::addVoxelStructure(VoxelStructure& structure) {
		voxelStructure = &structure;
	}

	void LightManager::drawVoxelStructure() {
		if (voxelStructure != nullptr)
			voxelStructure->build();
	}


	void LightManager::iterLights(std::function<void(Light&)> function) {
		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			DLightBinding& binding = *it;
			function(*binding.light);
		}

		for (auto it = pointLights.begin(); it != pointLights.end(); it++) {
			PLightBinding& binding = *it;
			function(*binding.light);
		}

		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			SLightBinding& binding = *it;
			function(*binding.light);
		}
	}

	void LightManager::iterDirectionalLights(std::function<void(DirectionalLight&)> function) {
		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			DLightBinding& binding = *it;
			function(*binding.light);
		}
	}

	void LightManager::iterPointLights(std::function<void(PointLight&)> function) {
		for (auto it = pointLights.begin(); it != pointLights.end(); it++) {
			PLightBinding& binding = *it;
			function(*binding.light);
		}
	}

	void LightManager::iterSpotLights(std::function<void(SpotLight&)> function) {
		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			SLightBinding& binding = *it;
			function(*binding.light);
		}
	}


	void LightManager::addLightAddListener(std::function<void(Light const *, ShadowMap const *)> listener) {
		addListener.push_back(listener);
	}

	void LightManager::addLightRemoveListener(std::function<void(Light const *, ShadowMap const *)> listener) {
		removeListener.push_back(listener);
	}

	void LightManager::addShaderListener(RenderShader& shader) {
		shaderListener.insert(&shader);
	}

	void LightManager::onAdd(Light* light, LightBinding& binding) {
		for (auto it = addListener.begin(); it != addListener.end(); it++) {
			auto function = *it;
			function(light, light->getShadowMap());
		}

		for (auto it = shaderListener.begin(); it != shaderListener.end(); it++) {
			RenderShader& shader = **it;
			light->addShadowmap(shader, binding.getName() + "shadowMap");
		}
	}

	void LightManager::onRemove(Light* light, LightBinding& binding) {
		for (auto it = removeListener.begin(); it != removeListener.end(); it++) {
			auto function = *it;
			function(light, light->getShadowMap());
		}

		for (auto it = shaderListener.begin(); it != shaderListener.end(); it++) {
			RenderShader& shader = **it;
			light->removeShadowmap(shader);
		}
	}


	template<class B, class L, class A>
	const B* LightManager::getBinding(const L& light, const A& list) const {
		const B* binding = nullptr;
		for (auto it = list.begin(); it != list.end(); it++) {
			const B& b = *it;

			if (&light == b.light) {
				binding = &b;
				break;
			}
		}

		return binding;
	}

	template<class B, class L, class A>
	B* LightManager::getBinding(const L& light, A& list) {
		B* binding = nullptr;
		for (auto it = list.begin(); it != list.end(); it++) {
			B& b = *it;

			if (&light == b.light) {
				binding = &b;
				break;
			}
		}

		return binding;
	}

	template<class A>
	void LightManager::reindexLights(A& list) const {
		size_t counter = 0;
		for (auto it = list.begin(); it != list.end(); it++) {
			LightBinding& b = *it;

			b.index = counter;
			counter++;
		}
	}

	const glm::vec3& LightManager::getAmbientColor() const {
		return ambient;
	}

	void LightManager::setAmbientColor(const glm::vec3& color) {
		ambient = color;
	}

}