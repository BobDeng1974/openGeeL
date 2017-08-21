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
				"renderer/shadowmapping/shadowmapping.frag")), plCount(0), dlCount(0), slCount(0) {}

	LightManager::~LightManager() {
		delete dlShader;
		delete plShader;

		iterLights([this](Light& light) {
			delete &light;
		});

		for (auto it = reflectionProbes.begin(); it != reflectionProbes.end(); it++)
			delete *it;
	}


	void LightManager::addLight(Light* light, LightBinding& binding) {
		lights[light] = std::move(binding);
		onAdd(light, lights[light]);

		light->addChangeListener([this](Light& light) { onChange(light); });
		light->addStatusListener([this](SceneObject& o, bool s) { reindexLights(); });
	}


	DirectionalLight& LightManager::addDirectionalLight(const SceneCamera& camera, Transform& transform, 
		vec3 diffuse, const ShadowMapConfiguration& config) {
		
		DirectionalLight* light = new DirectionalLight(transform, diffuse);
		LightBinding d = LightBinding(light, dlCount++, dlName);

		if (config.useShadowMap()) {
			//SimpleDirectionalLightMap* map = new SimpleDirectionalLightMap(*light, config.shadowBias, 100.f);
			CascadedDirectionalShadowMap* map = new CascadedDirectionalShadowMap(*light, camera, config.shadowBias, 1024, 1024);
			map->setIntensity(config.intensity);
			light->setShadowMap(*map);
		}

		addLight(light, d);
		return *light;
	}
	
	PointLight& LightManager::addPointLight(Transform& transform, vec3 diffuse, const ShadowMapConfiguration& config) {
		PointLight* light = new PointLight(transform, diffuse);
		LightBinding p = LightBinding(light, plCount++, plName);

		if (config.useShadowMap()) {
			SimplePointLightMap* map = new SimplePointLightMap(*light, config);
			map->setIntensity(config.intensity);
			light->setShadowMap(*map);
		}
		
		addLight(light, p);
		return *light;
	}

	SpotLight& LightManager::addSpotlight(Transform& transform, vec3 diffuse,
		float angle, float outerAngle, const ShadowMapConfiguration& config) {

		SpotLight* light = new SpotLight(transform, diffuse, angle, outerAngle);
		LightBinding s = LightBinding(light, slCount++, slName);
		
		if (config.useShadowMap()) {
			SimpleSpotLightMap* map = new SimpleSpotLightMap(*light, config);
			map->setIntensity(config.intensity);
			light->setShadowMap(*map);
		}

		addLight(light, s);
		return *light;
	}


	void LightManager::removeLight(Light& light) {
		auto it = lights.find(&light);
		if (it != lights.end()) {
			LightBinding& binding = it->second;

			onRemove(&light, binding);
			lights.erase(it);

			delete &light;
		}
	}

	

	void LightManager::bind(const RenderShader& shader, ShaderTransformSpace space, const Camera* const camera) const {
		iterLights([&](const LightBinding& binding) {
			Light& light = *binding.light;

			if (light.isActive())
				light.bind(shader, binding.getName(), space, camera);

		});

		shader.bind<int>(plCountName, plCount);
		shader.bind<int>(dlCountName, dlCount);
		shader.bind<int>(slCountName, slCount);
	}

	void LightManager::bind(const SceneShader& shader, const Camera* const camera) const {
		bind(shader, shader.getSpace(), camera);
	}


	void LightManager::bindShadowmaps(RenderShader& shader) const {
		iterLights([this, &shader](const LightBinding& binding) {
			Light& light = *binding.light;

			if(light.isActive())
				light.addShadowmap(shader, binding.getName());

		});
	}

	void LightManager::update(const RenderScene& scene, const SceneCamera* const camera) {
		drawShadowmaps(scene, camera);
		drawVoxelStructure();
	}


	void LightManager::drawShadowmaps(const RenderScene& scene, const SceneCamera* const camera) const {
		glCullFace(GL_BACK);

		iterLights([this, &scene, &camera](const LightBinding& binding) {
			Light& light = *binding.light;

			RenderShader* shader;
			switch (light.getLightType()) {
				case LightType::Point:
					shader = plShader;
					break;
				default:
					shader = dlShader;
					break;
			}

			if (light.isActive())
				light.renderShadowmap(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *shader);

		});

		glCullFace(GL_FRONT);
	}

	
	void LightManager::drawShadowmapsForced(const RenderScene& scene, const SceneCamera* const camera) const {
		glCullFace(GL_BACK);

		iterLights([this, &scene, &camera](const LightBinding& binding) {
			Light& light = *binding.light;

			RenderShader* shader;
			switch (light.getLightType()) {
				case LightType::Point:
					shader = plShader;
					break;
				default:
					shader = dlShader;
					break;
			}

			if (light.isActive())
				light.renderShadowmapForced(camera,
					[&](const RenderShader& shader) { scene.drawStaticObjects(shader); }, *shader);

		});

		glCullFace(GL_FRONT);
	}



	void LightManager::iterLights(std::function<void(Light&)> function) {
		for (auto it = lights.begin(); it != lights.end(); it++) {
			LightBinding& binding = it->second;
			function(*binding.light);
		}
	}

	void LightManager::iterLights(std::function<void(LightBinding&)> function) {
		for (auto it = lights.begin(); it != lights.end(); it++) {
			LightBinding& binding = it->second;
			function(binding);
		}
	}

	void LightManager::iterLights(std::function<void(const LightBinding&)> function) const {
		for (auto it = lights.begin(); it != lights.end(); it++) {
			const LightBinding& binding = it->second;
			function(binding);
		}
	}


	void LightManager::addShaderListener(RenderShader& shader) {
		bindShadowmaps(shader);
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
		reindexLights();

		for (auto it = shaderListener.begin(); it != shaderListener.end(); it++) {
			RenderShader& shader = **it;
			light->removeShadowmap(shader);
		}
	}

	void LightManager::onChange(Light& light) {

	}


	void LightManager::reindexLights() {

		//Remove shadow maps from all shader listeners since 
		//light index are changing
		for (auto it = shaderListener.begin(); it != shaderListener.end(); it++) {
			RenderShader& shader = **it;

			iterLights([this, &shader](Light& light) {
				light.removeShadowmap(shader);
			});
		}


		plCount = 0;
		dlCount = 0;
		slCount = 0;

		for (auto it = lights.begin(); it != lights.end(); it++) {
			LightBinding& b = it->second;
			Light& light = *b.light;

			size_t* counter;
			switch (light.getLightType()) {
				case LightType::Directional:
					counter = &dlCount;
					break;
				case LightType::Point:
					counter = &plCount;
					break;
				default:
					counter = &slCount;
					break;
			}

			if (light.isActive()) {
				b.index = *counter;
				(*counter)++;
			}
		}


		//Add newly indexed shadow maps to shaders again
		for (auto it = shaderListener.begin(); it != shaderListener.end(); it++) {
			RenderShader& shader = **it;

			iterLights([this, &shader](LightBinding& binding) {
				Light& light = *binding.light;
				light.addShadowmap(shader, binding.getName());
			});
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