#define GLEW_STATIC
#include <glew.h>
#include "lightmanager.h"
#include "light.h"
#include "pointlight.h"
#include "../shader/shader.h"

namespace geeL {

	LightManager::LightManager() {}

	LightManager::~LightManager() {
		for (size_t j = 0; j < staticPLs.size(); j++) {
			delete staticPLs[j];
		}

		for (size_t j = 0; j < dynamicPLs.size(); j++) {
			delete dynamicPLs[j];
		}
	}

	void LightManager::addLight(const PointLight* light) {
		if (light != nullptr && staticPLs.size() < MAX_POINTLIGHTS) {
			staticPLs.push_back(light);
		}
	}

	void LightManager::bind(const Shader& shader, string plName, string dlName, string slName,
		string plCountName, string dlCountName, string slCountName) const {

		shader.use();

		glUniform1i(glGetUniformLocation(shader.program, plCountName.c_str()), staticPLs.size());
		glUniform1i(glGetUniformLocation(shader.program, dlCountName.c_str()), staticDLs.size());
		glUniform1i(glGetUniformLocation(shader.program, slCountName.c_str()), staticSLs.size());

		for (size_t j = 0; j < staticPLs.size(); j++) {
			staticPLs[j]->bind(shader, j, plName);
		}
	}
}