#define GLEW_STATIC
#include <glew.h>
#include "lightmanager.h"
#include "light.h"
#include "pointlight.h"
#include "directionallight.h"
#include "spotlight.h"
#include "../shader/shader.h"
#include "../scene.h"

using namespace glm;

namespace geeL {

	LightManager::LightManager()
		: dlShader(new Shader("renderer/shaders/shadowmapping.vert", "renderer/shaders/empty.frag")) {}

	LightManager::~LightManager() {
		delete dlShader;

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

	DirectionalLight& LightManager::addLight(vec3 direction, vec3 diffuse, vec3 specular, vec3 ambient, float intensity) {
		DirectionalLight* light = new DirectionalLight(direction, diffuse, specular, ambient, intensity);
		staticDLs.push_back(light);
		light->initShadowmap();

		return *light;
	}

	
	PointLight& LightManager::addLight(vec3 position, vec3 diffuse, vec3 specular, vec3 ambient,
		float intensity, float attenuation) {

		PointLight* light = new PointLight(position, diffuse, specular, ambient, intensity);
		staticPLs.push_back(light);
		light->initShadowmap();
		
		return *light;
	}

	SpotLight& LightManager::addLight(vec3 position, vec3 direction, vec3 diffuse, vec3 specular, vec3 ambient,
		float angle, float outerAngle, float intensity) {

		SpotLight* light = new SpotLight(position, direction, diffuse, specular, 
			ambient, intensity, angle, outerAngle);
		staticSLs.push_back(light);
		light->initShadowmap();

		return *light;
	}

	void LightManager::bind(const Shader& shader, string plName,  string dlName, string slName, 
		string plCountName, string dlCountName, string slCountName) const {

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
			staticPLs[j]->addShadowmap(shader);
		}
		
		for (size_t j = 0; j < staticDLs.size(); j++) {
			staticDLs[j]->addShadowmap(shader);
		}

		for (size_t j = 0; j < staticSLs.size(); j++) {
			staticSLs[j]->addShadowmap(shader);
		}

		shader.bindMaps();
	}

	void LightManager::drawShadowmaps(const RenderScene& scene) const {
		for (size_t j = 0; j < staticPLs.size(); j++) {
			//TODO: change dlShader
			staticPLs[j]->renderShadowmap(scene, *dlShader);
		}

		for (size_t j = 0; j < staticDLs.size(); j++) {
			staticDLs[j]->renderShadowmap(scene, *dlShader);
		}

		for (size_t j = 0; j < staticSLs.size(); j++) {
			staticSLs[j]->renderShadowmap(scene, *dlShader);
		}
	}
}