#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <list>
#include <string>
#include <vec3.hpp>
#include <mat4x4.hpp>

#define GL_TEXTURE_2D 0x0DE1

typedef int ShaderLocation;
typedef unsigned int TextureID;

namespace geeL {

	struct TextureBinding {
		TextureID id;
		unsigned int type;
		unsigned int offset;
		std::string name;

		TextureBinding() {}

		TextureBinding(TextureID id, unsigned int type, unsigned offset, std::string name) :
			id(id), type(type), offset(offset), name(name) {}

		bool operator== (const TextureBinding &rhs) {
			return id == rhs.id;
		}
	};


	//Container class that creates and forwards information to linked GLSL shaders
	class Shader {

	public:
		int program;
		int mapBindingPos;
		const int mapOffset = 0;
		std::string name;

		Shader() : mapBindingPos(0) {}
		Shader(const char* vertexPath, const char* fragmentPath);
		Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);


		void use() const;

		//Add a new map to the shader
		void addMap(TextureID id, std::string name, unsigned int type = GL_TEXTURE_2D);

		//Remove map with given ID from shader (if it exists)
		void removeMap(TextureID id);

		//Load maps into their binding points in the shader
		void loadMaps() const;

		//Loads committed maps into the shader
		//IMPORTANT: no binding is taken care of, multiple calls will override previous one
		//and loading maps from other sources will also override this call
		void loadMaps(std::list<unsigned int>& maps, unsigned int type = GL_TEXTURE_2D) const;

		ShaderLocation getLocation(std::string name) const;

		ShaderLocation setInteger(std::string name, int value) const;
		ShaderLocation setFloat(std::string name, float value) const;
		ShaderLocation setVector3(std::string name, const glm::vec3& value) const;
		ShaderLocation setMat4(std::string name, const glm::mat4& value) const;

		void setInteger(ShaderLocation location, int value) const;
		void setFloat(ShaderLocation location, float value) const;
		void setVector3(ShaderLocation location, const glm::vec3& value) const;
		void setMat4(ShaderLocation location, const glm::mat4& value) const;

	protected:
		void init(const char* vertexPath, const char* fragmentPath);
		void init(const char* vertexPath, const char* geometryPath, const char* fragmentPath);

	private:
		std::map<TextureID, TextureBinding> maps;

		//Bind all added maps to the shader
		void bindMaps();

	};
}

#endif

