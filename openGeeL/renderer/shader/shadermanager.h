#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <map>

using namespace std;

namespace geeL {

class LightManager;
class Window;
class Shader;
class Camera;

class ShaderManager {

public:
	const Window* window;
	const LightManager* lightManager;

	ShaderManager(const LightManager* lightManager);
	~ShaderManager();

	void addShader(string name, const Shader* shader);
	const Shader* getShader(string name);

	void staticBind() const;
	void dynamicBind() const;

private:
	map<string, const Shader*> shaders;


};


}

#endif
