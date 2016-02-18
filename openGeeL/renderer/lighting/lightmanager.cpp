#include "lightmanager.h"
#include "light.h"
#include "../shader.h"

namespace geeL {

	LightManager::~LightManager() {
		for (size_t j = 0; j < lights.size(); j++) {
			delete lights[j];
		}
	}

	void LightManager::addLight(const Light* light) {
		if (light != nullptr && lights.size() < MAX_LIGHTS)
			lights.push_back(light);
	}

	void LightManager::addReceiver(const Shader* shader) {
		if (shader != nullptr)
			shaders.push_back(shader);
	}

	void LightManager::bind() const {
		for (size_t i = 0; i < shaders.size(); i++) {
			const Shader* shader = shaders[i];
			shader->use();
			
			for (size_t j = 0; j < lights.size(); j++) {
				lights[j]->bind(shader, j);
			}
		}
	}
}