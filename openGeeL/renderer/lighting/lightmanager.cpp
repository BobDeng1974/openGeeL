#define GLEW_STATIC
#include <glew.h>
#include "lightmanager.h"
#include "light.h"
#include "pointlight.h"
#include "directionallight.h"
#include "spotlight.h"
#include "../transformation/transform.h"
#include "../shader/shader.h"
#include "../scene.h"
#include <iostream>

using namespace std;
using namespace glm;

namespace geeL {

	LightManager::LightManager()
		: 
		dlShader(new Shader("renderer/shaders/shadowmapping.vert", "renderer/shaders/empty.frag")),
		plShader(new Shader("renderer/shaders/empty.vert", "renderer/shaders/shadowmapping.gs", 
			"renderer/shaders/shadowmapping.frag"))
		{}


	LightManager::~LightManager() {
		delete dlShader;
		delete plShader;

		for (size_t j = 0; j < staticPLs.size(); j++)
			delete staticPLs[j];

		for (size_t j = 0; j < dynamicPLs.size(); j++)
			delete dynamicPLs[j];
		
		for (size_t j = 0; j < staticDLs.size(); j++)
			delete staticDLs[j];

		for (size_t j = 0; j < dynamicDLs.size(); j++)
			delete dynamicDLs[j];

		for (size_t j = 0; j < staticSLs.size(); j++)
			delete staticSLs[j];

		for (size_t j = 0; j < dynamicSLs.size(); j++)
			delete dynamicSLs[j];
	}

	DirectionalLight& LightManager::addLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient, float intensity) {
		DirectionalLight* light = new DirectionalLight(transform, diffuse, specular, ambient, intensity);
		staticDLs.push_back(light);
		light->initShadowmap();

		return *light;
	}

	
	PointLight& LightManager::addLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient,
		float intensity, float attenuation) {

		PointLight* light = new PointLight(transform, diffuse, specular, ambient, intensity);
		staticPLs.push_back(light);
		light->initShadowmap();
		
		return *light;
	}

	SpotLight& LightManager::addLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient,
		float angle, float outerAngle, float intensity) {

		SpotLight* light = new SpotLight(transform, diffuse, specular,
			ambient, intensity, angle, outerAngle);
		staticSLs.push_back(light);
		light->initShadowmap();

		return *light;
	}

	void LightManager::bind(const Shader& shader) const {

		shader.use();

		glUniform1i(glGetUniformLocation(shader.program, plCountName.c_str()), staticPLs.size());
		glUniform1i(glGetUniformLocation(shader.program, dlCountName.c_str()), staticDLs.size());
		glUniform1i(glGetUniformLocation(shader.program, slCountName.c_str()), staticSLs.size());

		for (size_t j = 0; j < staticPLs.size(); j++) {
			staticPLs[j]->bind(shader, j, plName);
		}

		for (size_t j = 0; j < staticDLs.size(); j++) {
			staticDLs[j]->bind(shader, j, dlName);
		}

		for (size_t j = 0; j < staticSLs.size(); j++) {
			staticSLs[j]->bind(shader, j, slName);
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
}