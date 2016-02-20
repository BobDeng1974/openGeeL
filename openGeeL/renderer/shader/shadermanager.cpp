#include "shadermanager.h"
#include "../lighting/lightmanager.h"
#include "../shader/shader.h"
#include <iostream>

namespace geeL {

	typedef map<string, const Shader*>::const_iterator shadersIterator;

	ShaderManager::ShaderManager(const LightManager* lightManager) : lightManager(lightManager) {

	}

	ShaderManager::~ShaderManager() {
		for (std::map<string, const Shader*>::iterator it = shaders.begin(); it != shaders.end(); ++it)
			delete it->second;
	}

	void ShaderManager::addShader(string name, const Shader* shader) {
		if (shader != nullptr && !shaders.count(name))
			shaders[name] = shader;
	}

	const Shader* ShaderManager::getShader(string name) {
		return shaders[name];
	}

	void ShaderManager::staticBind() const {

	}

	void ShaderManager::dynamicBind() const {
		for (shadersIterator it = shaders.begin(); it != shaders.end(); it++) {
			lightManager->bind(*it->second);
		}
	}
}