#ifndef SHADER_H
#define SHADER_H

#include <list>
#include <string>
#include <utility>

namespace geeL {

class Shader {

public:
	int program;
	int mapBindingPos;
	const bool useLight;
	const bool useCamera;
	const bool useSkybox;
	std::string cam, skybox, point, spot, directional;

	Shader() : useLight(false), useCamera(false), useSkybox(false), cam("camera"), skybox("skybox"),
		point(""), spot(""), directional(""), mapBindingPos(0) {}

	Shader(const char* vertexPath, const char* fragmentPath, bool useLight = true, bool useCamera = true, bool useSkybox = true,
		std::string cam = "camera", std::string skybox = "skybox", std::string pointLight = "pointLights",
		std::string spotLights = "spotLights", std::string directionalLights = "directionalLights");

	void use() const;

	//Add a new map to the shader, e.g a shadow map
	void addMap(int mapID, std::string name);

	//Bind all added maps to the shader
	void bindMaps();

	//Load maps into their binding points in the shader
	void loadMaps() const;

private:
	std::list<std::pair<int, std::string>> maps;


};


}

#endif

