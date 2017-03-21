#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "genericmaterial.h"
#include "../shader/shader.h"
#include "../texturing/simpletexture.h"

using namespace std;

namespace geeL {

	GenericMaterialContainer::GenericMaterialContainer(string name, MaterialType type) : MaterialContainer(name, type) {
		textureStack = LayeredTexture();
	}

	void GenericMaterialContainer::addTexture(string name, TextureMap& texture) {
		textureStack.addTexture(this->name + "." + name, texture);
	}

	void GenericMaterialContainer::addTextures(vector<TextureMap*> textures) {
		for (size_t i = 0; i < textures.size(); i++) {
			TextureMap& texture = *textures[i];
			string name = this->name + "." + texture.getTypeAsString();

			textureStack.addTexture(name, texture);
		}
	}

	void GenericMaterialContainer::addParameter(string name, float parameter) {
		floatParameters[this->name + "." + name] = parameter;
	}

	void GenericMaterialContainer::addParameter(string name, int parameter) {
		intParameters[this->name + "." + name] = parameter;
	}

	void GenericMaterialContainer::addParameter(string name, vec3 parameter) {
		vec3Parameters[this->name + "." + name] = parameter;
	}

	void GenericMaterialContainer::addParameter(string name, mat4 parameter) {
		mat4Parameters[this->name + "." + name] = parameter;
	}


	void GenericMaterialContainer::bindTextures(SceneShader& shader) const {
		shader.use();
		textureStack.bind(shader, "");
	}

	void GenericMaterialContainer::bind(SceneShader& shader) const {
		shader.use();
		shader.loadMaps();
		textureStack.draw(shader);

		shader.setInteger("material.mapFlags", textureStack.mapFlags);
		shader.setFloat("material.type", (float)type);

		//GLint location;
		for (auto it = floatParameters.begin(); it != floatParameters.end(); it++) {
			pair<string, float> pair = *it;

			shader.setFloat(pair.first, pair.second);
		}

		for (auto it = intParameters.begin(); it != intParameters.end(); it++) {
			pair<string, int> pair = *it;
			shader.setInteger(pair.first, pair.second);
		}

		for (auto it = vec3Parameters.begin(); it != vec3Parameters.end(); it++) {
			pair<string, vec3> pair = *it;
			shader.setVector3(pair.first, pair.second);
		}

		for (auto it = mat4Parameters.begin(); it != mat4Parameters.end(); it++) {
			pair<string, mat4> pair = *it;
			shader.setMat4(pair.first, pair.second);
		}

	}

	float GenericMaterialContainer::getFloatValue(std::string name) const {
		if (floatParameters.count(name))
			return floatParameters.at(name);
		
		cout << "Value '" + name + "' not present in material\n";
		return 0.f;
	}

	int GenericMaterialContainer::getIntValue(std::string name) const {
		if (intParameters.count(name))
			return intParameters.at(name);

		cout << "Value '" + name + "' not present in material\n";
		return 0;
	}

	vec3 GenericMaterialContainer::getVectorValue(std::string name) const {
		if (vec3Parameters.count(name))
			return vec3Parameters.at(name);

		cout << "Value '" + name + "' not present in material\n";
		return vec3(0.f);
	}

	mat4 GenericMaterialContainer::getMatrixValue(std::string name) const {
		if (mat4Parameters.count(name))
			return mat4Parameters.at(name);

		cout << "Value '" + name + "' not present in material\n";
		return mat4(0.f);
	}

	void GenericMaterialContainer::setFloatValue(std::string name, float value) {
		floatParameters[name] = value;
	}

	void GenericMaterialContainer::setIntValue(std::string name, int value) {
		intParameters[name] = value;
	}

	void GenericMaterialContainer::setVectorValue(std::string name, const glm::vec3& value) {
		vec3Parameters[name] = value;
	}

	void GenericMaterialContainer::setMatrixValue(std::string name, mat4 value) {
		mat4Parameters[name] = value;
	}


}
