#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>

using namespace std;

namespace geeL {

class Shader {

public:
	int program;
	const bool useLight;
	const bool useCamera;
	const bool useSkybox;
	string cam, skybox, point, spot, directional;

	Shader() : useLight(false), useCamera(false), useSkybox(false), cam("camera"), skybox("skybox"),
		point(""), spot(""), directional("") {}

	Shader(const char* vertexPath, const char* fragmentPath, bool useLight = true, bool useCamera = true, bool useSkybox = true,
		string cam = "camera", string skybox = "skybox", string pointLight = "pointLights", 
		string spotLights = "spotLights", string directionalLights = "directionalLights");

	void use() const;

};


}

#endif

