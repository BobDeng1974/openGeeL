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
#include "../scene.h"
#include <iostream>

using namespace std;
using namespace glm;

namespace geeL {

	LightManager::LightManager(glm::vec3 ambient)
		: 
		ambient(ambient),
		dlShader(new Shader("renderer/shaders/shadowmapping.vert", "renderer/shaders/empty.frag")),
		plShader(new Shader("renderer/shaders/empty.vert", "renderer/shaders/shadowmapping.gs", 
			"renderer/shaders/shadowmapping.frag"))
		{}


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

	DirectionalLight& LightManager::addDirectionalLight(Transform& transform, vec3 diffuse, float shadowBias) {
		DirectionalLight* light = new DirectionalLight(transform, diffuse, shadowBias);
		staticDLs.push_back(light);
		light->initShadowmap();

		return *light;
	}

	
	PointLight& LightManager::addPointLight(Transform& transform, vec3 diffuse, float shadowBias) {

		PointLight* light = new PointLight(transform, diffuse, shadowBias);
		staticPLs.push_back(light);
		light->initShadowmap();
		
		return *light;
	}

	SpotLight& LightManager::addSpotlight(Transform& transform, vec3 diffuse,
		float angle, float outerAngle, float shadowBias) {

		SpotLight* light = new SpotLight(transform, diffuse, angle, outerAngle, shadowBias);
		staticSLs.push_back(light);
		light->initShadowmap();

		return *light;
	}

	void LightManager::deferredBind(const RenderScene& scene, const Shader& shader) const {
		shader.use();

		shader.setInteger(plCountName, staticPLs.size());
		shader.setInteger(dlCountName, staticDLs.size());
		shader.setInteger(slCountName, staticSLs.size());

		for (size_t j = 0; j < staticPLs.size(); j++)
			staticPLs[j]->deferredBind(scene, shader, plName + "[" + std::to_string(j) + "].");

		for (size_t j = 0; j < staticDLs.size(); j++)
			staticDLs[j]->deferredBind(scene, shader, dlName + "[" + std::to_string(j) + "].");

		for (size_t j = 0; j < staticSLs.size(); j++)
			staticSLs[j]->deferredBind(scene, shader, slName + "[" + std::to_string(j) + "].");
	}

	void LightManager::forwardBind(const Shader& shader) const {
		shader.use();

		shader.setInteger(plCountName, staticPLs.size());
		shader.setInteger(dlCountName, staticDLs.size());
		shader.setInteger(slCountName, staticSLs.size());

		for (size_t j = 0; j < staticPLs.size(); j++) {
			staticPLs[j]->forwardBind(shader, plName + "[" + std::to_string(j) + "].", "");
		}

		for (size_t j = 0; j < staticDLs.size(); j++) {
			staticDLs[j]->forwardBind(shader, dlName + "[" + std::to_string(j) + "].", 
				"direLightMatrix[" + to_string(j) + "]");
		}
			

		for (size_t j = 0; j < staticSLs.size(); j++) {
			staticSLs[j]->forwardBind(shader, slName + "[" + std::to_string(j) + "].", 
				"spotLightMatrix[" + to_string(j) + "]");
		}
			
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
		}

		shader.bindMaps();
	}

	void LightManager::forwardScreenInfo(const ScreenInfo& info, const Camera& camera) {

		for (size_t j = 0; j < staticDLs.size(); j++) {
			staticDLs[j]->computeLightTransformExt(info, camera.center);
		}
	}

	void LightManager::drawShadowmaps(const RenderScene& scene) const {
		for (size_t j = 0; j < staticPLs.size(); j++) {
			staticPLs[j]->renderShadowmap(scene, *plShader);
		}

		for (size_t j = 0; j < staticDLs.size(); j++) {
			staticDLs[j]->renderShadowmap(scene, *dlShader);
		}

		for (size_t j = 0; j < staticSLs.size(); j++) {
			staticSLs[j]->renderShadowmap(scene, *dlShader);
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