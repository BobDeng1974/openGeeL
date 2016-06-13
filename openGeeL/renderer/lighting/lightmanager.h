#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <vector>

using namespace std;

#define MAX_POINTLIGHTS 5
#define MAX_DIRECTIONALLIGHTS 5
#define MAX_SPOTLIGHTS 5

namespace geeL {

class Light;
class PointLight;
class Shader;

class LightManager {

public:
	LightManager();

	//Manager is responsible for removing the lights
	~LightManager();

	void addLight(const PointLight* light);

	void bind(const Shader& shader, string plName = "pointLights", string dlName = "directionalLights", string slName = "spotLights",
		string plCountName = "plCount", string dlCountName = "dlCount", string slCountName = "slCount") const;

private:
	vector<const Light*> staticPLs, staticDLs, staticSLs;
	vector<const Light*> dynamicPLs, dynamicDLs, dynamicSLs;
};

}

#endif
