#define GLEW_STATIC
#include <glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../texturing/texture.h"
#include "shader.h"

using namespace std;

namespace geeL {

	static unsigned int activeShader = 0;
	void Shader::use() const {
		if (program != activeShader) {
			glUseProgram(program);
			activeShader = program;
		}
	}

	void Shader::addMap(TextureID id, const std::string& name, unsigned int type) {
		auto it = maps.find(name);
		//Update texture ID if a binding with same name already exists
		if (it != maps.end()) {
			TextureBinding& binding = it->second;
			binding.id = id;
		}
		//Add new texture binding otherwise
		else {
			use();
			unsigned int offset = maps.size();
			glUniform1i(glGetUniformLocation(program, name.c_str()), mapOffset + offset);

			maps[name] = TextureBinding(id, type, offset, name);
			mapBindingPos = maps.size() + mapOffset;
		}
	}

	void Shader::addMap(const Texture& texture, const std::string& name, unsigned int type) {
		addMap(texture.getID(), name, type);
	}

	void Shader::removeMap(TextureID id) {
		auto element = maps.end();
		for (auto it = maps.begin(); it != maps.end(); it++) {
			TextureBinding& tex = (*it).second;

			if (tex.id == id) {
				element = it;
				break;
			}
		}

		if (element != maps.end()) {
			maps.erase(element);

			//Rebind all maps again since positions might have changed
			bindMaps();
		}
	}

	void Shader::removeMap(const std::string& name) {
		auto it = maps.find(name);
		if (it != maps.end()) {
			maps.erase(it);

			//Rebind all maps again since positions might have changed
			bindMaps();
		}
	}

	TextureID Shader::getMap(const std::string& name) const {
		auto it = maps.find(name);
		if (it != maps.end())
			return (*it).second.id;

		return 0;
	}

	void Shader::bindMaps() {
		int counter = 0;
		for (auto it = maps.begin(); it != maps.end(); it++) {
			TextureBinding& binding = (*it).second;
			binding.offset = counter;
			glUniform1i(glGetUniformLocation(program, binding.name.c_str()), mapOffset + binding.offset);
			counter++;
		}

		mapBindingPos = maps.size() + mapOffset;
	}

	void Shader::loadMaps() const {
		int layer = GL_TEXTURE0;
		for (auto it = maps.begin(); it != maps.end(); it++) {
			const TextureBinding& binding = (*it).second;
			glActiveTexture(layer + mapOffset + binding.offset);
			glBindTexture(binding.type, binding.id);
		}
	}

	void Shader::loadMaps(std::list<TextureID>& maps, unsigned int type) const {
		int layer = GL_TEXTURE0;
		int counter = mapOffset;
		for (auto it = maps.begin(); it != maps.end(); it++) {
			glActiveTexture(layer + counter);
			glBindTexture(type, *it);
			counter++;
		}
	}

	ShaderLocation Shader::getLocation(std::string name) const {
		return glGetUniformLocation(program, name.c_str());
	}

	ShaderLocation Shader::setInteger(string name, int value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform1i(glGetUniformLocation(program, name.c_str()), value);

		return location;
	}

	ShaderLocation Shader::setFloat(string name, float value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform1f(glGetUniformLocation(program, name.c_str()), value);

		return location;
	}

	ShaderLocation Shader::setVector2(string name, const glm::vec2& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform2f(glGetUniformLocation(program, name.c_str()),
			value.x, value.y);

		return location;
	}

	ShaderLocation Shader::setVector3(string name, const glm::vec3& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform3f(glGetUniformLocation(program, name.c_str()),
			value.x, value.y, value.z);

		return location;
	}

	ShaderLocation Shader::setMat4(string name, const glm::mat4& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE,
			glm::value_ptr(value));

		return location;
	}

	void Shader::setInteger(ShaderLocation location, int value) const {
		glUniform1i(location, value);
	}

	void Shader::setFloat(ShaderLocation location, float value) const {
		glUniform1f(location, value);
	}

	void Shader::setVector2(ShaderLocation location, const glm::vec2& value) const {
		glUniform2f(location, value.x, value.y);
	}

	void Shader::setVector3(ShaderLocation location, const glm::vec3& value) const {
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::setMat4(ShaderLocation location, const glm::mat4& value) const {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

