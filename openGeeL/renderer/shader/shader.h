#ifndef SHADER_H
#define SHADER_H

#include <list>
#include <string>
#include <vec3.hpp>
#include <mat4x4.hpp>

#define GL_TEXTURE_2D 0x0DE1


namespace geeL {

	struct TextureBinding {
		unsigned int id;
		unsigned int type;
		std::string name;

		TextureBinding(unsigned int id, unsigned int type, std::string name) :
			id(id), type(type), name(name) {}
	};


	//Container class that creates and forwards information to linked GLSL shaders
	class Shader {

	public:
		int program;
		int mapBindingPos;
		int mapOffset = 0;
		std::string name;

		Shader() : mapBindingPos(0) {}
		Shader(const char* vertexPath, const char* fragmentPath);
		Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);


		void use() const;

		//Add a new map to the shader, e.g a shadow map
		void addMap(unsigned int mapID, std::string name, unsigned int type = GL_TEXTURE_2D);

		//Bind all added maps to the shader
		void bindMaps();

		//Load maps into their binding points in the shader
		void loadMaps() const;

		//Loads committed maps into the shader
		//IMPORTANT: no binding is taken care of, multiple calls will override previous one
		//and loading maps from other sources will also override this call
		void loadMaps(std::list<unsigned int>& maps, unsigned int type = GL_TEXTURE_2D) const;

		void setInteger(std::string name, int value) const;
		void setFloat(std::string name, float value) const;
		void setVector3(std::string name, const glm::vec3& value) const;
		void setMat4(std::string name, const glm::mat4& value) const;

	private:
		std::list<TextureBinding> maps;

	};


	//Special shader container that is intented for the use in scene drawing.
	//It therefore holds additional scene information.
	class SceneShader : public Shader {

	public:
		const bool deferred;
		const bool useLight;
		const bool useCamera;
		const bool useSkybox;
		std::string cameraName, skyboxName;


		SceneShader();

		SceneShader(const char* vertexPath, const char* fragmentPath, bool deferred = false, bool useLight = true, bool useCamera = true,
			bool useSkybox = true, std::string cameraName = "camera", std::string skyboxName = "skybox");

		SceneShader(const char* vertexPath, const char* geometryPath, const char* fragmentPath, bool deferred = false, bool useLight = true,
			bool useCamera = true, bool useSkybox = true, std::string cameraName = "camera", std::string skyboxName = "skybox");

	};
}

#endif

