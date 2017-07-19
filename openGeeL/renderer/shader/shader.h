#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <list>
#include <string>
#include <vec2.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "../texturing/texturetype.h"

typedef int ShaderLocation;

namespace geeL {

	class Texture;

	struct TextureBinding {
		const Texture* texture;
		unsigned int offset;
		std::string name;

		TextureBinding() {}

		TextureBinding(const Texture* texture, unsigned offset, std::string name) :
			texture(texture), offset(offset), name(name) {}

		bool operator== (const TextureBinding &rhs);

	};


	//Container class that creates and forwards information to linked GLSL shaders
	class Shader {

	public:
		unsigned int mapBindingPos;
		unsigned int mapOffset = 0;
		std::string name;

		~Shader();

		void use() const;
		unsigned int getProgram() const;

		//Add a new map to the shader
		void addMap(const Texture& texture, const std::string& name);
		const Texture * const getMap(const std::string& name) const;

		//Remove map from shader (if it is attached)
		void removeMap(const Texture& texture);

		//Remove map with given name from shader (if it exists)
		void removeMap(const std::string& name);

		//Load maps into their binding points in the shader
		virtual void loadMaps() const;

		//Loads committed maps into the shader
		//IMPORTANT: no binding is taken care of, multiple calls will override previous one
		//and loading maps from other sources will also override this call
		void loadMaps(std::list<unsigned int>& maps, TextureType type = TextureType::Texture2D) const;

		ShaderLocation getLocation(const std::string& name) const;

		ShaderLocation setInteger(const std::string& name, int value) const;
		ShaderLocation setFloat(const std::string& name, float value) const;
		ShaderLocation setVector2(const std::string& name, const glm::vec2& value) const;
		ShaderLocation setVector3(const std::string& name, const glm::vec3& value) const;
		ShaderLocation setVector4(const std::string& name, const glm::vec4& value) const;
		ShaderLocation setMat3(const std::string& name, const glm::mat3& value) const;
		ShaderLocation setMat4(const std::string& name, const glm::mat4& value) const;

		void setInteger(ShaderLocation location, int value) const;
		void setFloat(ShaderLocation location, float value) const;
		void setVector2(ShaderLocation location, const glm::vec2& value) const;
		void setVector3(ShaderLocation location, const glm::vec3& value) const;
		void setVector4(ShaderLocation location, const glm::vec4& value) const;
		void setMat3(ShaderLocation location, const glm::mat3& value) const;
		void setMat4(ShaderLocation location, const glm::mat4& value) const;

	protected:
		unsigned int program;
		std::map<std::string, TextureBinding> maps;

		Shader() : mapBindingPos(0) {}

	private:
		//Bind all added maps to the shader
		void bindMaps();

	};


	inline unsigned int Shader::getProgram() const {
		return program;
	}


}

#endif


