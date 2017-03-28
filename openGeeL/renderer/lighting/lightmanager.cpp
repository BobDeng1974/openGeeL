#define GLEW_STATIC
#include <glew.h>
#include "lightmanager.h"
#include "light.h"
#include "pointlight.h"
#include "directionallight.h"
#include "spotlight.h"
#include "../cameras/camera.h"
#include "../utility/gbuffer.h"
#include "../transformation/transform.h"
#include "../shader/shader.h"
#include "../shader/sceneshader.h"
#include "../shadowmapping/shadowmap.h"
#include "../shadowmapping/simpleshadowmap.h"
#include "../shadowmapping/cascadedmap.h"
#include "../scene.h"

using namespace std;
using namespace glm;

namespace geeL {

	LightManager::LightManager(glm::vec3 ambient) : ambient(ambient),
		dlShader(new Shader("renderer/shaders/shadowmapping.vert", "renderer/shaders/empty.frag")),
		plShader(new Shader("renderer/shaders/empty.vert", "renderer/shaders/shadowmapping.gs", 
			"renderer/shaders/shadowmapping.frag")) {}


	LightManager::~LightManager() {
		delete dlShader;
		delete plShader;

		for (auto it = pointLights.begin(); it != pointLights.end(); it++)
			delete (*it).light;

		for (auto it = dirLights.begin(); it != dirLights.end(); it++)
			delete (*it).light;

		for (auto it = spotLights.begin(); it != spotLights.end(); it++)
			delete (*it).light;
	}

	DirectionalLight& LightManager::addDirectionalLight(const Camera& camera, Transform& transform, vec3 diffuse, float shadowBias) {
		DirectionalLight* light = new DirectionalLight(transform, diffuse);
		
		size_t index = dirLights.size();
		DLightBinding d = DLightBinding(light, index, dlName);
		dirLights.push_back(std::move(d));

		CascadedDirectionalShadowMap* map = new CascadedDirectionalShadowMap(*light, camera, shadowBias, 512, 512);
		light->setShadowMap(*map);

		onAdd(light);
		return *light;
	}

	
	PointLight& LightManager::addPointLight(Transform& transform, vec3 diffuse, float shadowBias) {
		PointLight* light = new PointLight(transform, diffuse);
		
		size_t index = pointLights.size();
		PLightBinding p = PLightBinding(light, index, plName);
		pointLights.push_back(std::move(p));

		SimplePointLightMap* map = new SimplePointLightMap(*light, shadowBias, 100.f);
		light->setShadowMap(*map);
		
		onAdd(light);
		return *light;
	}

	SpotLight& LightManager::addSpotlight(Transform& transform, vec3 diffuse,
		float angle, float outerAngle, float shadowBias) {

		SpotLight* light = new SpotLight(transform, diffuse, angle, outerAngle);

		size_t index = spotLights.size();
		SLightBinding s = SLightBinding(light, index, slName);
		spotLights.push_back(std::move(s));

		SimpleSpotLightMap* map = new SimpleSpotLightMap(*light, shadowBias, 100.f);
		light->setShadowMap(*map);
		
		onAdd(light);
		return *light;
	}

	void LightManager::removeLight(DirectionalLight& light) {
		//Find corresponding light binding
		DLightBinding* binding = getBinding<DLightBinding, DirectionalLight,
			std::list<DLightBinding>>(light, dirLights);

		if (binding != nullptr) {
			dirLights.remove(*binding);
			reindexLights<std::list<DLightBinding>>(dirLights);

			onRemove(&light);
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

			onRemove(&light);
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

			onRemove(&light);
			delete &light;
		}
	}


	void LightManager::bind(const RenderScene& scene, const Shader& shader, ShaderTransformSpace space) const {
		shader.use();

		int plCount = 0;
		int dlCount = 0;
		int slCount = 0;

		for (auto it = pointLights.begin(); it != pointLights.end(); it++) {
			const PLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive()) {
				light.bind(scene, shader, binding.getName(plCount), space);
				plCount++;
			}
		}

		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			const DLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive()) {
				light.bind(scene, shader, binding.getName(dlCount), space);
				dlCount++;
			}
		}
			
		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			const SLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive()) {
				light.bind(scene, shader, binding.getName(slCount), space);
				slCount++;
			}
		}
		
		shader.setInteger(plCountName, plCount);
		shader.setInteger(dlCountName, dlCount);
		shader.setInteger(slCountName, slCount);
	}

	void LightManager::bind(const RenderScene& scene, const SceneShader& shader) const {
		bind(scene, shader, shader.getSpace());
	}


	void LightManager::bindShadowmap(Shader& shader, PointLight& light) {
		//Find corresponding light binding
		PLightBinding* binding = getBinding<PLightBinding, PointLight,
			std::list<PLightBinding>>(light, pointLights);

		//Bind shadowmap
		if(binding != nullptr)
			light.addShadowmap(shader, binding->getName() + "shadowMap");
	}

	void LightManager::bindShadowmap(Shader& shader, SpotLight& light) {
		//Find corresponding light binding
		SLightBinding* binding = getBinding<SLightBinding, SpotLight, 
			std::list<SLightBinding>>(light, spotLights);

		//Bind shadowmap
		if (binding != nullptr)
			light.addShadowmap(shader, binding->getName() + "shadowMap");
	}

	void LightManager::bindShadowmap(Shader& shader, DirectionalLight& light) {
		//Find corresponding light binding
		DLightBinding* binding = getBinding<DLightBinding, DirectionalLight,
			std::list<DLightBinding>>(light, dirLights);

		//Bind shadowmap
		if (binding != nullptr)
			light.addShadowmap(shader, binding->getName() + "shadowMap");
	}

	void LightManager::bindShadowmaps(Shader& shader) const {
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

	void LightManager::drawShadowmaps(const RenderScene& scene) const {

		for(auto it = pointLights.begin(); it != pointLights.end(); it++) {
			const PLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmap(scene.getCamera(),
					[&](const Shader& shader) { scene.drawStaticObjects(shader); }, *plShader);
		}

		for (auto it = dirLights.begin(); it != dirLights.end(); it++) {
			const DLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmap(scene.getCamera(),
					[&](const Shader& shader) { scene.drawStaticObjects(shader); }, *dlShader);
		}

		for (auto it = spotLights.begin(); it != spotLights.end(); it++) {
			const SLightBinding& binding = *it;
			Light& light = *binding.light;
			if (light.isActive())
				light.renderShadowmap(scene.getCamera(),
					[&](const Shader& shader) { scene.drawStaticObjects(shader); }, *dlShader);
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

	void LightManager::onAdd(Light* light) {
		for (auto it = addListener.begin(); it != addListener.end(); it++) {
			auto function = *it;
			function(light, light->getShadowMap());
		}
	}

	void LightManager::onRemove(Light* light) {
		for (auto it = removeListener.begin(); it != removeListener.end(); it++) {
			auto function = *it;
			function(light, light->getShadowMap());
		}

	}


	template<class B, class L, class A>
	B* LightManager::getBinding(L& light, A& list) {
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
	void LightManager::reindexLights(A& list) {
		size_t counter = 0;
		for (auto it = list.begin(); it != list.end(); it++) {
			LightBinding& b = *it;

			b.index = counter;
			counter++;
		}
	}


}