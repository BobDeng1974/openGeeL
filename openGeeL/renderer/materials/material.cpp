#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "material.h"
#include "../shader/shader.h"
#include "../texturing/simpletexture.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"

using namespace std;

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

	void Material::addParameter(string name, const float* parameter) {
		unmanagedFloatParameters.push_back(pair<string, const float*>(this->name + "." + name, parameter));
	}

	void Material::addParameter(string name, const int* parameter) {
		unmanagedIntParameters.push_back(pair<string, const int*>(this->name + "." + name, parameter));
	}

	void Material::addParameter(string name, const vec3* parameter) {
		unmanagedVec3Parameters.push_back(pair<string, const vec3*>(this->name + "." + name, parameter));
	}

	void Material::addParameter(string name, const mat4* parameter) {
		unmanagedMat4Parameters.push_back(pair<string, const mat4*>(this->name + "." + name, parameter));
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
		for (list<pair<string, float>>::const_iterator it = floatParameters.begin(); it != floatParameters.end(); it++) {
			pair<string, float> pair = *it;

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform1f(location, pair.second);
		}

		for (list<pair<string, const float*>>::const_iterator it = unmanagedFloatParameters.begin(); it != unmanagedFloatParameters.end(); it++) {
			pair<string, const float*> pair = *it;

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform1f(location, *pair.second);
		}

		for (list<pair<string, int>>::const_iterator it = intParameters.begin(); it != intParameters.end(); it++) {
			pair<string, int> pair = *it;

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform1i(location, pair.second);
		}

		for (list<pair<string, vec3>>::const_iterator it = vec3Parameters.begin(); it != vec3Parameters.end(); it++) {
			pair<string, vec3> pair = *it;

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform3f(location, pair.second.x, pair.second.y, pair.second.z);
		}

		for (list<pair<string, const vec3*>>::const_iterator it = unmanagedVec3Parameters.begin(); it != unmanagedVec3Parameters.end(); it++) {
			pair<string, const vec3*> pair = *it;

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform3f(location, pair.second->x, pair.second->y, pair.second->z);
		}

		for (list<pair<string, mat4>>::const_iterator it = mat4Parameters.begin(); it != mat4Parameters.end(); it++) {
			pair<string, mat4> pair = *it;

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(pair.second));
		}

		for (list<pair<string, const mat4*>>::const_iterator it = unmanagedMat4Parameters.begin(); it != unmanagedMat4Parameters.end(); it++) {
			pair<string, const mat4*> pair = *it;

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(*pair.second));
		}

	}

}