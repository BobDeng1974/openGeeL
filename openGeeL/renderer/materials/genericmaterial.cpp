#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "genericmaterial.h"
#include "../shader/shader.h"
#include "../texturing/simpletexture.h"

using namespace std;

namespace geeL {

	GenericMaterial::GenericMaterial(Shader& shader, string name, MaterialType type) : Material(shader, name, type) {
		textureStack = LayeredTexture();
	}

	void GenericMaterial::addTexture(string name, SimpleTexture& texture) {
		textureStack.addTexture(this->name + "." + name, texture);
	}

	void GenericMaterial::addTextures(vector<SimpleTexture*> textures) {
		for (size_t i = 0; i < textures.size(); i++) {
			SimpleTexture& texture = *textures[i];
			string name = this->name + "." + texture.GetTypeAsString();

			textureStack.addTexture(name, texture);
		}
	}

	void GenericMaterial::addParameter(string name, float parameter) {
		floatParameters.push_back(pair<string, float>(this->name + "." + name, parameter));
	}

	void GenericMaterial::addParameter(string name, int parameter) {
		intParameters.push_back(pair<string, int>(this->name + "." + name, parameter));
	}

	void GenericMaterial::addParameter(string name, vec3 parameter) {
		vec3Parameters.push_back(pair<string, vec3>(this->name + "." + name, parameter));
	}

	void GenericMaterial::addParameter(string name, mat4 parameter) {
		mat4Parameters.push_back(pair<string, mat4>(this->name + "." + name, parameter));
	}

	void GenericMaterial::addParameter(string name, const float* parameter) {
		unmanagedFloatParameters.push_back(pair<string, const float*>(this->name + "." + name, parameter));
	}

	void GenericMaterial::addParameter(string name, const int* parameter) {
		unmanagedIntParameters.push_back(pair<string, const int*>(this->name + "." + name, parameter));
	}

	void GenericMaterial::addParameter(string name, const vec3* parameter) {
		unmanagedVec3Parameters.push_back(pair<string, const vec3*>(this->name + "." + name, parameter));
	}

	void GenericMaterial::addParameter(string name, const mat4* parameter) {
		unmanagedMat4Parameters.push_back(pair<string, const mat4*>(this->name + "." + name, parameter));
	}

	void GenericMaterial::bindTextures() const {
		shader.use();
		textureStack.bind(shader, "");
	}

	void GenericMaterial::bind() const {
		shader.use();
		shader.loadMaps();
		textureStack.draw(shader);

		shader.setInteger("material.mapFlags", textureStack.mapFlags);
		shader.setFloat("material.type", type);

		GLint location;
		for (list<pair<string, float>>::const_iterator it = floatParameters.begin(); it != floatParameters.end(); it++) {
			pair<string, float> pair = *it;

			shader.setFloat(pair.first, pair.second);
		}

		for (list<pair<string, const float*>>::const_iterator it = unmanagedFloatParameters.begin(); it != unmanagedFloatParameters.end(); it++) {
			pair<string, const float*> pair = *it;
			shader.setFloat(pair.first, *pair.second);
		}

		for (list<pair<string, int>>::const_iterator it = intParameters.begin(); it != intParameters.end(); it++) {
			pair<string, int> pair = *it;
			shader.setInteger(pair.first, pair.second);
		}

		for (list<pair<string, const int*>>::const_iterator it = unmanagedIntParameters.begin(); it != unmanagedIntParameters.end(); it++) {
			pair<string, const int*> pair = *it;
			shader.setInteger(pair.first, *pair.second);
		}

		for (list<pair<string, vec3>>::const_iterator it = vec3Parameters.begin(); it != vec3Parameters.end(); it++) {
			pair<string, vec3> pair = *it;
			shader.setVector3(pair.first, pair.second);
		}

		for (list<pair<string, const vec3*>>::const_iterator it = unmanagedVec3Parameters.begin(); it != unmanagedVec3Parameters.end(); it++) {
			pair<string, const vec3*> pair = *it;
			shader.setVector3(pair.first, *pair.second);
		}

		for (list<pair<string, mat4>>::const_iterator it = mat4Parameters.begin(); it != mat4Parameters.end(); it++) {
			pair<string, mat4> pair = *it;
			shader.setMat4(pair.first, pair.second);
		}

		for (list<pair<string, const mat4*>>::const_iterator it = unmanagedMat4Parameters.begin(); it != unmanagedMat4Parameters.end(); it++) {
			pair<string, const mat4*> pair = *it;
			shader.setMat4(pair.first, *pair.second);
		}
	}

}
