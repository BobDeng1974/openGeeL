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

	Shader::~Shader() {
		glDeleteProgram(program);

		for (auto it = shaderBindings.begin(); it != shaderBindings.end(); it++)
			delete it->second;
	}


	void Shader::addMap(const Texture& texture, const std::string& name) {
		auto it = maps.find(name);
		//Update texture ID if a binding with same name already exists
		if (it != maps.end()) {
			TextureBinding& binding = it->second;
			binding.texture = &texture;
		}
		//Add new texture binding otherwise
		else {
			use();
			unsigned int offset = maps.size();
			glUniform1i(glGetUniformLocation(program, name.c_str()), mapOffset + offset);

			maps[name] = TextureBinding(&texture, offset, name);
			mapBindingPos = maps.size() + mapOffset;
		}
	}

	void Shader::removeMap(const Texture& texture) {
		auto element = maps.end();
		for (auto it = maps.begin(); it != maps.end(); it++) {
			TextureBinding& tex = (*it).second;

			if (*tex.texture == texture) {
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

	const Texture * const Shader::getMap(const std::string& name) const {
		auto it = maps.find(name);
		if (it != maps.end())
			return (*it).second.texture;

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
			binding.texture->bind();
		}
	}

	void Shader::loadMaps(std::list<unsigned int>& maps, TextureType type) const {
		int textureType = (int)type;

		int layer = GL_TEXTURE0;
		int counter = mapOffset;
		for (auto it = maps.begin(); it != maps.end(); it++) {
			glActiveTexture(layer + counter);
			glBindTexture(textureType, *it);
			counter++;
		}
	}

	ShaderLocation Shader::getLocation(const string& name) const {
		return glGetUniformLocation(program, name.c_str());
	}


	ShaderLocation Shader::bind(const string& name, int value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform1i(location, value);

		return location;
	}

	ShaderLocation Shader::bind(const string& name, float value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform1f(location, value);

		return location;
	}

	ShaderLocation Shader::bind(const string& name, const glm::vec2& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform2f(location, value.x, value.y);

		return location;
	}

	ShaderLocation Shader::bind(const string& name, const glm::vec3& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);

		return location;
	}

	ShaderLocation Shader::bind(const std::string & name, const glm::vec4& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);

		return location;
	}

	ShaderLocation Shader::bind(const string& name, const glm::mat3& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));

		return location;
	}

	ShaderLocation Shader::bind(const string& name, const glm::mat4& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));

		return location;
	}

	void Shader::bind(ShaderLocation location, const int& value) const {
		glUniform1i(location, value);
	}

	void Shader::bind(ShaderLocation location, const float& value) const {
		glUniform1f(location, value);
	}

	void Shader::bind(ShaderLocation location, const glm::vec2& value) const {
		glUniform2f(location, value.x, value.y);
	}

	void Shader::bind(ShaderLocation location, const glm::vec3& value) const {
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::bind(ShaderLocation location, const glm::vec4& value) const {
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Shader::bind(ShaderLocation location, const glm::mat3 & value) const {
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::bind(ShaderLocation location, const glm::mat4& value) const {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}


	void Shader::setValue(const std::string& name, float value, Range<float> range) {
		FloatBinding* binding;

		auto& it = shaderBindings.find(name);
		if (it != shaderBindings.end()) {
			binding = static_cast<FloatBinding*>(it->second);
			
			bool set = binding->setValue(value);
			if(set) bindingQueue.push(binding);
		}
		else {
			binding = new FloatBinding(*this, name, value, range);
			shaderBindings[name] = binding;

			bindingQueue.push(binding);
		}
	}

	void Shader::setValue(const std::string& name, int value, Range<int> range) {
		IntegerBinding* binding;

		auto it = shaderBindings.find(name);
		if (it != shaderBindings.end()) {
			binding = static_cast<IntegerBinding*>(it->second);

			bool set = binding->setValue(value);
			if (set) bindingQueue.push(binding);
		}
		else {
			binding = new IntegerBinding(*this, name, value, range);
			shaderBindings[name] = binding;

			bindingQueue.push(binding);
		}
	}

	float Shader::getFloatValue(const std::string& name) const {
		auto it = shaderBindings.find(name);
		if (it != shaderBindings.end()) {
			FloatBinding* binding = static_cast<FloatBinding*>(it->second);

			return binding->getValue();
		}

		return 0.f;
	}

	int Shader::getIntValue(const std::string& name) const {
		auto it = shaderBindings.find(name);
		if (it != shaderBindings.end()) {
			IntegerBinding* binding = static_cast<IntegerBinding*>(it->second);

			return binding->getValue();
		}

		return 0;
	}

	void Shader::bindParameters() {
		use();

		while (!bindingQueue.empty()) {
			ShaderBinding* binding = bindingQueue.front();
			binding->bind();

			bindingQueue.pop();
		}
	}

	void Shader::iterateBindings(std::function<void(const ShaderBinding&)> function) {
		for (auto it(shaderBindings.begin()); it != shaderBindings.end(); it++)
			function(*it->second);
	}


}

