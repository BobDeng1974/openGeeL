#define GLEW_STATIC
#include <glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "texturing/texture.h"
#include "shader.h"

using namespace std;

namespace geeL {

	unsigned int Shader::activeProgram = 0;
	void Shader::use() const {
		if (program != Shader::activeProgram) {
			glUseProgram(program);
			Shader::activeProgram = program;
		}
	}

	Shader::~Shader() {
		glDeleteProgram(program);

		for (auto it = shaderBindings.begin(); it != shaderBindings.end(); it++)
			delete it->second;
	}


	void Shader::addMap(const ITexture& texture, const std::string& name) {
		auto it = maps.find(name);
		//Update texture ID if a binding with same name already exists
		if (it != maps.end()) {
			TextureBinding& binding = it->second;
			binding.texture = &texture;
		}
		//Add new texture binding otherwise
		else {
			if (maps.size() >= TextureBindingStack::MAX_TEXTURE_BINDINGS) {
				std::cout << "Can't add more than " << TextureBindingStack::MAX_TEXTURE_BINDINGS
					<< "texture to shader\n";

				return;
			}

			use();
			unsigned int offset = maps.size();
			bind<int>(name, mapOffset + offset);

			maps[name] = TextureBinding(&texture, offset, name);
			mapBindingPos = maps.size() + mapOffset;
		}
	}

	void Shader::removeMap(const ITexture& texture) {
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

	const ITexture* const Shader::getMap(const std::string& name) const {
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
			bind<int>(binding.name, mapOffset + binding.offset);
			counter++;
		}

		mapBindingPos = maps.size() + mapOffset;
	}

	void Shader::loadMaps() const {
		TextureBindingStack::bindTexturesSimple(*this, mapOffset);
	}


	void Shader::loadMapsDynamic() const {
		TextureBindingStack::bindTexturesDynamic(*this, mapOffset);
	}

	void Shader::initDraw() const {
		//use();
		loadMaps();
	}

	ShaderLocation Shader::getLocation(const string& name) const {
		return glGetUniformLocation(program, name.c_str());
	}


	ShaderLocation Shader::bind(const std::string& name, bool value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform1i(location, value);

		return location;
	}

	ShaderLocation Shader::bind(const string& name, int value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform1i(location, value);

		return location;
	}

	ShaderLocation Shader::bind(const std::string& name, unsigned int value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform1ui(location, value);

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

	ShaderLocation Shader::bind(const std::string & name, const gvec2& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform2f(location, value.x, value.y);

		return location;
	}

	ShaderLocation Shader::bind(const std::string & name, const gvec3& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);

		return location;
	}

	ShaderLocation Shader::bind(const std::string & name, const gvec4& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);

		return location;
	}

	ShaderLocation Shader::bind(const std::string & name, const gmat3& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glm::mat3& mat = Properties::convertMat<glm::mat3, gmat3>(value);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));

		return location;
	}

	ShaderLocation Shader::bind(const std::string & name, const gmat4& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glm::mat4& mat = Properties::convertMat<glm::mat4, gmat4>(value);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));

		return location;
	}

	void Shader::bind(ShaderLocation location, const bool& value) const {
		glUniform1i(location, value);
	}

	void Shader::bind(ShaderLocation location, const int& value) const {
		glUniform1i(location, value);
	}

	void Shader::bind(ShaderLocation location, const unsigned int & value) const {
		glUniform1ui(location, value);
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

	void Shader::bind(ShaderLocation location, const glm::mat3& value) const {
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::bind(ShaderLocation location, const glm::mat4& value) const {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::bind(ShaderLocation location, const gvec2& value) const {
		glUniform2f(location, value.x, value.y);
	}

	void Shader::bind(ShaderLocation location, const gvec3& value) const {
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::bind(ShaderLocation location, const gvec4& value) const {
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Shader::bind(ShaderLocation location, const gmat3& value) const {
		glm::mat3& mat = Properties::convertMat<glm::mat3, gmat3>(value);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void Shader::bind(ShaderLocation location, const gmat4& value) const {
		glm::mat4& mat = Properties::convertMat<glm::mat4, gmat4>(value);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
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

		//Update values of internal bindings
		while (!bindingQueue.empty()) {
			ShaderBinding* binding = bindingQueue.front();
			binding->bind();

			bindingQueue.pop();
		}

		//Update values of external bindings (e.g. of properties)
		//and delete binding afterwards
		while (!tempQueue.empty()) {
			ShaderBinding* binding = tempQueue.front();
			binding->bind();

			bindingQueue.pop();
			delete binding;
		}
	}

	void Shader::iterateBindings(std::function<void(const ShaderBinding&)> function) {
		for (auto it(shaderBindings.begin()); it != shaderBindings.end(); it++)
			function(*it->second);
	}

	void Shader::iterateTextures(std::function<void(const TextureBinding&)> function) const {
		for (auto it(maps.begin()); it != maps.end(); it++) {
			const TextureBinding& binding = (*it).second;
			function(binding);
		}
	}


}

