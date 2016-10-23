#ifndef SHADER_H
#define SHADER_H

#include <list>
#include <string>
#include <utility>

#define GL_TEXTURE_2D 0x0DE1

namespace geeL {

struct TextureBinding {
	unsigned int id;
	unsigned int type;
	std::string name;

	TextureBinding(unsigned int id, unsigned int type, std::string name) :
		id(id), type(type), name(name) {}
};


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

	Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath, bool useLight = true, bool useCamera = true, bool useSkybox = true,
		std::string cam = "camera", std::string skybox = "skybox", std::string pointLight = "pointLights",
		std::string spotLights = "spotLights", std::string directionalLights = "directionalLights");

	void use() const;

	//Add a new map to the shader, e.g a shadow map
	void addMap(unsigned int mapID, std::string name, unsigned int type = GL_TEXTURE_2D);

	//Bind all added maps to the shader
	void bindMaps();

	//Load maps into their binding points in the shader
	void loadMaps() const;

private:
	std::list<TextureBinding> maps;


};


}

#endif

