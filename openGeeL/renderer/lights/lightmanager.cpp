#define GLEW_STATIC
#include <glew.h>

#include "cameras/camera.h"
#include "framebuffer/gbuffer.h"
#include "transformation/transform.h"
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "shadowmapping/shadowmap.h"
#include "shadowmapping/simpleshadowmap.h"
#include "shadowmapping/varianceshadowmap.h"
#include "shadowmapping/cascadedmap.h"
#include "shadowmapping/shadowmapadapter.h"
#include "shadowmapping/shadowmapstack.h"
#include "cubemapping/reflectionprobe.h"
#include "cubemapping/iblmap.h"
#include "framebuffer/cubebuffer.h"
#include "glwrapper/glguards.h"
#include "voxelization/voxelstructure.h"
#include "renderscene.h"
#include "light.h"
#include "lightbinding.h"
#include "lightmanager.h"

using namespace std;
using namespace glm;

namespace geeL {

	LightManager::LightManager(ShadowmapStack& shadowmapStack)
		: ambient(glm::vec3(0.f))
		, voxelStructure(nullptr)
		, shadowmapStack(shadowmapStack)
		, mapAdapter(nullptr)
		, plCount(0)
		, dlCount(0)
		, slCount(0) {}

	LightManager::~LightManager() {
		iterLights([this](Light& light) {
			delete &light;
		});
	}


	void LightManager::addLight(Light* light, LightBinding& binding) {
		lights[light] = std::move(binding);
		onAdd(light, lights[light]);

		light->addChangeListener([this](Light& light) { onChange(light); });
		light->addStatusListener([this](SceneObject& o, bool s) { reindexLights(); });
		light->addShadowmapChangeListener([this](Light& light) { onMapChange(light); });
	}

	void LightManager::addDirectionalLightInternal(DirectionalLight* light, const ShadowMapConfiguration& config) {
		LightBinding d(light, dlCount++, dlName);

		if (config.useShadowMap()) {
			SimpleDirectionalLightMap* map = new SimpleDirectionalLightMap(*light, config);
			//CascadedDirectionalShadowmap* map = new CascadedDirectionalShadowmap(*light, *camera, config.shadowBias, config.resolution);
			map->setIntensity(config.intensity);
			light->attachShadowmap(unique_ptr<Shadowmap>(map));
		}

		addLight(light, d);
	}

	void LightManager::addPointLightInternal(PointLight* light, const ShadowMapConfiguration& config) {
		LightBinding p(light, plCount++, plName);

		if (config.useShadowMap()) {
			SimplePointLightMap* map = new SimplePointLightMap(*light, config);
			map->setIntensity(config.intensity);
			light->attachShadowmap(unique_ptr<Shadowmap>(map));
		}

		addLight(light, p);
	}

	void LightManager::addSpotlightInternal(SpotLight* light, const ShadowMapConfiguration& config) {
		LightBinding s(light, slCount++, slName);

		if (config.useShadowMap()) {
			Shadowmap* map = new SimpleSpotLightMap(*light, config);
			map->setIntensity(config.intensity);
			light->attachShadowmap(unique_ptr<Shadowmap>(map));
		}

		addLight(light, s);
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

	

	void LightManager::bind(const Shader& shader, ShaderTransformSpace space, const Camera* const camera) const {
		iterLights([&](const LightBinding& binding) {
			Light& light = *binding.light;

			if (light.isActive())
				light.bind(shader, binding.getName(), space, camera);

		});

		shader.bind<int>(plCountName, int(plCount));
		shader.bind<int>(dlCountName, int(dlCount));
		shader.bind<int>(slCountName, int(slCount));
	}

	void LightManager::bind(const SceneShader& shader, const Camera* const camera) const {
		bind(shader, shader.getSpace(), camera);
	}


	void LightManager::bindShadowmaps(Shader& shader) const {
		shadowmapStack.staticBind(shader);
	}

	void LightManager::update(const RenderScene& scene, const SceneCamera* const camera) {
		if (mapAdapter != nullptr)
			mapAdapter->update();

		drawShadowmaps(scene, camera);
		drawVoxelStructure();


		shadowmapStack.update();

		for (auto it = shaderListener.begin(); it != shaderListener.end(); it++) {
			Shader& shader = **it;
			shadowmapStack.bind(shader);
		}

		shadowmapStack.lateUpdate();
	}


	void LightManager::drawShadowmaps(const RenderScene& scene, const SceneCamera* const camera) {
		CullingGuard guard(CullingMode::cullBack);

		shadowmapStack.iterActiveShadowmaps([this, &scene, &camera](Light& light) {
			light.renderShadowmap(camera, scene, shaderRepository);
		});
	}

	
	void LightManager::drawShadowmapsForced(const RenderScene& scene, const SceneCamera* const camera) {
		CullingGuard guard(CullingMode::cullBack);

		iterLights([this, &scene, &camera](const LightBinding& binding) {
			Light& light = *binding.light;

			if (light.isActive())
				light.renderShadowmapForced(camera, scene, shaderRepository);
		});
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

	void LightManager::iterShadowmaps(std::function<void(Shadowmap&)> function) const {
		for (auto it = lights.begin(); it != lights.end(); it++) {
			const LightBinding& binding = it->second;
			Light& light = *binding.light;

			Shadowmap* map = light.getShadowMap();

			if (map != nullptr)
				function(*map);
		}
	}


	void LightManager::addShaderListener(Shader& shader) {
		bindShadowmaps(shader);
		shaderListener.insert(&shader);
	}

	void LightManager::addShaderListener(SceneShader& shader) {
		bind(shader, camera);

		RenderShader& s = shader;
		addShaderListener(s);
	}


	void LightManager::onAdd(Light* light, LightBinding& binding) {

	}

	
	void LightManager::onRemove(Light* light, LightBinding& binding) {
		reindexLights();
		shadowmapStack.remove(*light);
	}

	void LightManager::onChange(Light& light) {

	}

	void LightManager::onMapChange(Light& light) {
		if (light.hasActiveMaps())
			shadowmapStack.add(light);
		else
			shadowmapStack.remove(light);
	}


	void LightManager::reindexLights() {
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
	}

	const glm::vec3& LightManager::getAmbientColor() const {
		return ambient;
	}

	void LightManager::setAmbientColor(const glm::vec3& color) {
		ambient = color;
	}




	DynamicIBLMap& LightManager::addReflectionProbe(DynamicIBLMap& probe) {
		reflectionProbes.push_back(&probe);
		return probe;
	}

	IBLMap& LightManager::addReflectionProbe(IBLMap& probe) {
		reflectionProbes.push_back(&probe);
		return probe;
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

	void LightManager::removeReflectionProbes(RenderShader& shader) const {
		for (auto it = reflectionProbes.begin(); it != reflectionProbes.end(); it++) {
			DynamicCubeMap& probe = **it;
			probe.remove(shader);
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

	ShadowmapAdapter* const LightManager::getShadowmapAdapter() {
		return mapAdapter;
	}

	void LightManager::addShadowmapAdapter(ShadowmapAdapter& manager) {
		mapAdapter = &manager;
	}



}