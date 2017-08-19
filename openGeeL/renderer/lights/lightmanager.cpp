#define GLEW_STATIC
#include <glew.h>
#include "pointlight.h"
#include "directionallight.h"
#include "spotlight.h"
#include "cameras/camera.h"
#include "framebuffer/gbuffer.h"
#include "transformation/transform.h"
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "shadowmapping/shadowmap.h"
#include "shadowmapping/simpleshadowmap.h"
#include "shadowmapping/cascadedmap.h"
#include "cubemapping/reflectionprobe.h"
#include "cubemapping/iblmap.h"
#include "framebuffer/cubebuffer.h"
#include "voxelization/voxelstructure.h"
#include "renderscene.h"
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

		iterLights([this](Light& light) {
			delete &light;
		});

		for (auto it = reflectionProbes.begin(); it != reflectionProbes.end(); it++)
			delete *it;
	}

	DirectionalLight& LightManager::addDirectionalLight(const SceneCamera& camera, Transform& transform, 
		vec3 diffuse, const ShadowMapConfiguration& config) {
		
		DirectionalLight* light = new DirectionalLight(transform, diffuse);
		
		size_t index = dirLights.size();
		LightBinding d = LightBinding(light, index, dlName);
		dirLights[light] = std::move(d);

		if (config.useShadowMap()) {
			//SimpleDirectionalLightMap* map = new SimpleDirectionalLightMap(*light, config.shadowBias, 100.f);
			CascadedDirectionalShadowMap* map = new CascadedDirectionalShadowMap(*light, camera, config.shadowBias, 1024, 1024);
			light->setShadowMap(*map);
		}

		light->addChangeListener([this](const Light& light) { onChange(light); });
		onAdd(light, dirLights[light]);
		return *light;
	}

	
	PointLight& LightManager::addPointLight(Transform& transform, vec3 diffuse, const ShadowMapConfiguration& config) {
		PointLight* light = new PointLight(transform, diffuse);
		
		size_t index = pointLights.size();
		LightBinding p = LightBinding(light, index, plName);
		pointLights[light] = std::move(p);

		if (config.useShadowMap()) {
			SimplePointLightMap* map = new SimplePointLightMap(*light, config);
			light->setShadowMap(*map);
		}
		
		light->addChangeListener([this](const Light& light) { onChange(light); });
		onAdd(light, pointLights[light]);
		return *light;
	}

	SpotLight& LightManager::addSpotlight(Transform& transform, vec3 diffuse,
		float angle, float outerAngle, const ShadowMapConfiguration& config) {

		SpotLight* light = new SpotLight(transform, diffuse, angle, outerAngle);

		size_t index = spotLights.size();
		LightBinding s = LightBinding(light, index, slName);
		spotLights[light] = std::move(s);

		if (config.useShadowMap()) {
			SimpleSpotLightMap* map = new SimpleSpotLightMap(*light, config);
			light->setShadowMap(*map);
		}
		
		light->addChangeListener([this](const Light& light) { onChange(light); });
		onAdd(light, spotLights[light]);
		return *light;
	}


	void LightManager::removeLight(DirectionalLight& light) {

		auto it = dirLights.find(&light);
		if (it != dirLights.end()) {
			LightBinding& binding = it->second;

			reindexLights<std::map<DirectionalLight*, LightBinding>>(dirLights);
			onRemove(&light, binding);
			dirLights.erase(it);

			delete &light;
		}
	}

	void LightManager::removeLight(PointLight& light) {

		auto it = pointLights.find(&light);
		if (it != pointLights.end()) {
			LightBinding& binding = it->second;

			reindexLights<std::map<PointLight*, LightBinding>>(pointLights);
			onRemove(&light, binding);
			pointLights.erase(it);

			delete &light;
		}
	}


	void LightManager::removeLight(SpotLight& light) {

		auto it = spotLights.find(&light);
		if (it != spotLights.end()) {
			LightBinding& binding = it->second;

			reindexLights<std::map<SpotLight*, LightBinding>>(spotLights);
			onRemove(&light, binding);
			spotLights.erase(it);

			delete &light;
		}
	}

	

	void LightManager::bind(const RenderShader& shader, ShaderTransformSpace space, const Camera* const camera) const {
		unsigned int plCount = 0;
		unsigned int dlCount = 0;
		unsigned int slCount = 0;

		for (auto it = pointLights.begin(); it != pointLights.end(); it++) {
			const LightBinding& binding = it->second;
			Light& light = *binding.light;
			if (light.isActive()) {
				light.bind(shader, binding.getName(plCount), space, camera);
				plCount++;
			}
		}

		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			const LightBinding& binding = it->second;
			Light& light = *binding.light;
			if (light.isActive()) {
				light.bind(shader, binding.getName(dlCount), space, camera);
				dlCount++;
			}
		}
			
		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			const LightBinding& binding = it->second;
			Light& light = *binding.light;
			if (light.isActive()) {
				light.bind(shader, binding.getName(slCount), space, camera);
				slCount++;
			}
		}
		
		shader.bind<int>(plCountName, plCount);
		shader.bind<int>(dlCountName, dlCount);
		shader.bind<int>(slCountName, slCount);
	}

	void LightManager::bind(const SceneShader& shader, const Camera* const camera) const {
		bind(shader, shader.getSpace(), camera);
	}


	void LightManager::bindShadowMaps(RenderShader& shader) const {

		for (auto it = pointLights.begin(); it != pointLights.end(); it++) {
			const LightBinding& binding = it->second;
			Light& light = *it->first;
			light.addShadowmap(shader, binding.getName());
		}
		
		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			const LightBinding& binding = it->second;
			Light& light = *binding.light;
			light.addShadowmap(shader, binding.getName());
		}

		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			const LightBinding& binding = it->second;
			Light& light = *binding.light;
			light.addShadowmap(shader, binding.getName());
		}
	}

	void LightManager::draw(const RenderScene& scene, const SceneCamera* const camera) {
		drawShadowmaps(scene, camera);
		drawVoxelStructure();
	}


	void LightManager::drawShadowmaps(const RenderScene& scene, const SceneCamera* const camera) const {
		glCullFace(GL_BACK);

		for(auto it = pointLights.begin(); it != pointLights.end(); it++) {
			const LightBinding& binding = it->second;
			Light& light = *it->first;
			if (light.isActive())
				light.renderShadowmap(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *plShader);
		}

		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			const LightBinding& binding = it->second;
			Light& light = *it->first;
			if (light.isActive())
				light.renderShadowmap(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *dlShader);
		}

		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			const LightBinding& binding = it->second;
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
			const LightBinding& binding = it->second;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmapForced(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *plShader);
		}

		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			const LightBinding& binding = it->second;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmapForced(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *dlShader);
		}

		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			const LightBinding& binding = it->second;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmapForced(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *dlShader);
		}

		glCullFace(GL_FRONT);
	}






	void LightManager::iterLights(std::function<void(Light&)> function) {
		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			LightBinding& binding = it->second;
			function(*binding.light);
		}

		for (auto it = pointLights.begin(); it != pointLights.end(); it++) {
			LightBinding& binding = it->second;
			function(*binding.light);
		}

		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			LightBinding& binding = it->second;
			function(*binding.light);
		}
	}


	void LightManager::addShaderListener(RenderShader& shader) {
		bindShadowMaps(shader);
		shaderListener.insert(&shader);
	}

	void LightManager::addShaderListener(SceneShader& shader) {
		bind(shader, camera);

		RenderShader& s = shader;
		addShaderListener(s);
	}


	void LightManager::onAdd(Light* light, LightBinding& binding) {
		for (auto it = shaderListener.begin(); it != shaderListener.end(); it++) {
			RenderShader& shader = **it;
			light->addShadowmap(shader, binding.getName());
		}
	}

	void LightManager::onRemove(Light* light, LightBinding& binding) {
		for (auto it = shaderListener.begin(); it != shaderListener.end(); it++) {
			RenderShader& shader = **it;
			light->removeShadowmap(shader);
		}
	}

	void LightManager::onChange(const Light& light) {

	}



	template<class A>
	void LightManager::reindexLights(A& list) const {
		size_t counter = 0;
		for (auto it = list.begin(); it != list.end(); it++) {
			LightBinding& b = it->second;

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
		for (auto it = reflectionProbes.begin(); it != reflectionProbes.end(); it++) {
			DynamicCubeMap& probe = **it;
			probe.add(shader, "skybox.");
		}
	}

	void LightManager::bindReflectionProbes(const Camera& camera, const RenderShader& shader, ShaderTransformSpace space) const {

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
			probe.draw();
		}
	}


	void LightManager::addVoxelStructure(VoxelStructure& structure) {
		voxelStructure = &structure;
	}

	void LightManager::drawVoxelStructure() {
		if (voxelStructure != nullptr)
			voxelStructure->build();
	}



}