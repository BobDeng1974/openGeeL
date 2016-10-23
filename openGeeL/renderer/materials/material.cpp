#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "material.h"
#include "../shader/shader.h"
#include "../texturing/simpletexture.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"

namespace geeL {

	Material::Material(Shader& shader, string name, MaterialType type) : shader(shader), type(type), name(name) {
		textureStack = LayeredTexture();
	}

	void Material::addTexture(string name, SimpleTexture& texture) {
		textureStack.addTexture(this->name + "." + name, texture);
	}

	void Material::addTextures(vector<SimpleTexture*> textures) {

		for (size_t i = 0; i < textures.size(); i++) {
			SimpleTexture& texture = *textures[i];
			string name = this->name + "." + texture.GetTypeAsString();

			textureStack.addTexture(name, texture);
		}
	}

	void Material::addParameter(string name, float parameter) {
		floatParameters.push_back(pair<string, float>(this->name + "." + name, parameter));
	}

	void Material::addParameter(string name, int parameter) {
		intParameters.push_back(pair<string, int>(this->name + "." + name, parameter));
	}

	void Material::addParameter(string name, vec3 parameter) {
		vec3Parameters.push_back(pair<string, vec3>(this->name + "." + name, parameter));
	}

	void Material::addParameter(string name, mat4 parameter) {
		mat4Parameters.push_back(pair<string, mat4>(this->name + "." + name, parameter));
	}

	void Material::bindTextures() const {
		shader.use();
		textureStack.bind(shader, "");
	}

	void Material::bind() const {
		shader.use();
		shader.loadMaps();
		textureStack.draw(shader);

		glUniform1i(glGetUniformLocation(shader.program, "material.mapFlags"), textureStack.mapFlags);
		glUniform1f(glGetUniformLocation(shader.program, "material.type"), type);

		GLint location;
		for (size_t i = 0; i < floatParameters.size(); i++) {
			pair<string, float> pair = floatParameters[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform1f(location, pair.second);
		}

		for (size_t i = 0; i < intParameters.size(); i++) {
			pair<string, int> pair = intParameters[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform1i(location, pair.second);
		}

		for (size_t i = 0; i < vec3Parameters.size(); i++) {
			pair<string, vec3> pair = vec3Parameters[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform3f(location, pair.second.x, pair.second.y, pair.second.z);
		}

		for (size_t i = 0; i < mat4Parameters.size(); i++) {
			pair<string, mat4> pair = mat4Parameters[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(pair.second));
		}
	}

}