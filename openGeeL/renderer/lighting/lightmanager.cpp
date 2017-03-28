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

		for (size_t j = 0; j < staticPLs.size(); j++)
			delete staticPLs[j];

		for (size_t j = 0; j < staticDLs.size(); j++)
			delete staticDLs[j];

		for (size_t j = 0; j < staticSLs.size(); j++)
			delete staticSLs[j];
	}

	DirectionalLight& LightManager::addDirectionalLight(const Camera& camera, Transform& transform, vec3 diffuse, float shadowBias) {
		DirectionalLight* light = new DirectionalLight(transform, diffuse);
		staticDLs.push_back(light);

		CascadedDirectionalShadowMap* map = new CascadedDirectionalShadowMap(*light, camera, shadowBias, 512, 512);
		light->setShadowMap(*map);

		return *light;
	}

	
	PointLight& LightManager::addPointLight(Transform& transform, vec3 diffuse, float shadowBias) {
		PointLight* light = new PointLight(transform, diffuse);
		staticPLs.push_back(light);

		SimplePointLightMap* map = new SimplePointLightMap(*light, shadowBias, 100.f);
		light->setShadowMap(*map);
		
		return *light;
	}

	SpotLight& LightManager::addSpotlight(Transform& transform, vec3 diffuse,
		float angle, float outerAngle, float shadowBias) {

		SpotLight* light = new SpotLight(transform, diffuse, angle, outerAngle);
		staticSLs.push_back(light);

		SimpleSpotLightMap* map = new SimpleSpotLightMap(*light, shadowBias, 100.f);
		light->setShadowMap(*map);

		return *light;
	}

	void LightManager::bind(const RenderScene& scene, const Shader& shader, ShaderTransformSpace space) const {
		shader.use();

		int plCount = 0;
		int dlCount = 0;
		int slCount = 0;

		for (size_t j = 0; j < staticPLs.size(); j++) {
			if (staticPLs[j]->isActive()) {
				staticPLs[j]->bind(scene, shader, plName + "[" + std::to_string(j) + "].", space);
				plCount++;
			}
		}

		for (size_t j = 0; j < staticDLs.size(); j++) {
			if (staticDLs[j]->isActive()) {
				staticDLs[j]->bind(scene, shader, dlName + "[" + std::to_string(j) + "].", space);
				dlCount++;
			}
		}
			
		for (size_t j = 0; j < staticSLs.size(); j++) {
			if (staticSLs[j]->isActive()) {
				staticSLs[j]->bind(scene, shader, slName + "[" + std::to_string(j) + "].", space);
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

	void LightManager::bindShadowmaps(Shader& shader) const {
		shader.use();

		for (size_t j = 0; j < staticPLs.size(); j++) {
			string name = plName + "[" + to_string(j) + "].shadowMap";
			staticPLs[j]->addShadowmap(shader, name);
		}
		
		for (size_t j = 0; j < staticDLs.size(); j++) {
			string name = dlName + "[" + to_string(j) + "].shadowMap";
			staticDLs[j]->addShadowmap(shader, name);
		}

		for (size_t j = 0; j < staticSLs.size(); j++) {
			string name = slName + "[" + to_string(j) + "].shadowMap";
			staticSLs[j]->addShadowmap(shader, name);

			name = slName + "[" + to_string(j) + "].cookie";
			staticSLs[j]->addLightCookie(shader, name);
		}
	}

	void LightManager::drawShadowmaps(const RenderScene& scene) const {

		for (size_t j = 0; j < staticPLs.size(); j++) {
			if (staticPLs[j]->isActive())
				staticPLs[j]->renderShadowmap(scene.getCamera(), 
					[&](const Shader& shader) { scene.drawStaticObjects(shader); }, *plShader);
		}

		for (size_t j = 0; j < staticDLs.size(); j++) {
			if (staticDLs[j]->isActive())
				staticDLs[j]->renderShadowmap(scene.getCamera(),
					[&](const Shader& shader) { scene.drawStaticObjects(shader); }, *dlShader);
		}

		for (size_t j = 0; j < staticSLs.size(); j++) {
			if (staticSLs[j]->isActive())
				staticSLs[j]->renderShadowmap(scene.getCamera(),
					[&](const Shader& shader) { scene.drawStaticObjects(shader); }, *dlShader);
		}	
	}

	std::vector<DirectionalLight*>::iterator LightManager::directionalLightsBegin() {
		return staticDLs.begin();
	}

	std::vector<DirectionalLight*>::iterator LightManager::directionalLightsEnd() {
		return staticDLs.end();
	}

	std::vector<SpotLight*>::iterator LightManager::spotLightsBegin() {
		return staticSLs.begin();
	}

	std::vector<SpotLight*>::iterator LightManager::spotLightsEnd() {
		return staticSLs.end();
	}

	std::vector<PointLight*>::iterator LightManager::pointLightsBegin() {
		return staticPLs.begin();
	}

	std::vector<PointLight*>::iterator LightManager::pointLightsEnd() {
		return staticPLs.end();
	}
}