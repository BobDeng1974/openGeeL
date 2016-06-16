#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <string>

using namespace std;

namespace geeL {

class Camera;
class LightManager;
class MaterialFactory;
class Shader;

class ShaderManager {

public:
	ShaderManager(const MaterialFactory& factory);

	void staticBind(const LightManager& lightManager, const Camera& currentCamera) const;
	void dynamicBind(const LightManager& lightManager, const Camera& currentCamera) const;

private:
	const MaterialFactory& factory;

};

}

#endif
