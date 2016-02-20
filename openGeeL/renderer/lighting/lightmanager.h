#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <glm.hpp>
#include <vector>

using namespace glm;

#define MAX_LIGHTS 10

namespace geeL {

class Light;
class Shader;

class LightManager {

public:
	LightManager() {}

	//Manager is responsible for removing the lights
	~LightManager();

	void addLight(const Light* light);
	void addReceiver(const Shader& shader);
	void bind() const;
	void bind(const Shader& shader) const;

private:
	std::vector<const Light*> lights;
	std::vector<const Shader*> shaders;

};

}

#endif
