#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "genericmaterial.h"
#include "shader/sceneshader.h"
#include "texturing/imagetexture.h"

using namespace std;

namespace geeL {

	GenericMaterialContainer::GenericMaterialContainer(const std::string& name)
		: MaterialContainer(name) {}


	void GenericMaterialContainer::addTextureUnit(const std::string & name) {
		auto it = textures.find(this->name + "." + name);
		if (it == textures.end())
			it->second = nullptr;
	}

	void GenericMaterialContainer::addTexture(const std::string& name, MemoryObject<ITexture> texture) {
		textures[this->name + "." + name] = texture;
	}


	void GenericMaterialContainer::addParameter(const std::string& name, float parameter) {
		floatParameters[this->name + "." + name] = parameter;
	}

	void GenericMaterialContainer::addParameter(const std::string& name, int parameter) {
		intParameters[this->name + "." + name] = parameter;
	}

	void GenericMaterialContainer::addParameter(const std::string& name, vec3 parameter) {
		vec3Parameters[this->name + "." + name] = parameter;
	}

	void GenericMaterialContainer::addParameter(const std::string& name, mat4 parameter) {
		mat4Parameters[this->name + "." + name] = parameter;
	}


	void GenericMaterialContainer::bind(SceneShader& shader) const {
		for (auto it(textures.begin()); it != textures.end(); it++) {
			MemoryObject<ITexture> texture = it->second;
			std::string boundName = it->first + "Bound";

			if (texture != nullptr) {
				shader.addMap(*texture, it->first);
				shader.bind<int>(boundName, 1);
			}
			else
				shader.bind<int>(boundName, 0);
		}

		for (auto it = floatParameters.begin(); it != floatParameters.end(); it++) {
			pair<string, float> pair = *it;

			shader.bind<float>(pair.first, pair.second);
		}

		for (auto it = intParameters.begin(); it != intParameters.end(); it++) {
			pair<string, int> pair = *it;
			shader.bind<int>(pair.first, pair.second);
		}

		for (auto it = vec3Parameters.begin(); it != vec3Parameters.end(); it++) {
			pair<string, vec3> pair = *it;
			shader.bind<glm::vec3>(pair.first, pair.second);
		}

		for (auto it = mat4Parameters.begin(); it != mat4Parameters.end(); it++) {
			pair<string, mat4> pair = *it;
			shader.bind<glm::mat4>(pair.first, pair.second);
		}

	}

	float GenericMaterialContainer::getFloatValue(const std::string& name) const {
		if (floatParameters.count(name))
			return floatParameters.at(name);
		
		cout << "Value '" + name + "' not present in material\n";
		return 0.f;
	}

	int GenericMaterialContainer::getIntValue(const std::string& name) const {
		if (intParameters.count(name))
			return intParameters.at(name);

		cout << "Value '" + name + "' not present in material\n";
		return 0;
	}

	vec3 GenericMaterialContainer::getVectorValue(const std::string& name) const {
		if (vec3Parameters.count(name))
			return vec3Parameters.at(name);

		cout << "Value '" + name + "' not present in material\n";
		return vec3(0.f);
	}

	mat4 GenericMaterialContainer::getMatrixValue(const std::string& name) const {
		if (mat4Parameters.count(name))
			return mat4Parameters.at(name);

		cout << "Value '" + name + "' not present in material\n";
		return mat4(0.f);
	}

	void GenericMaterialContainer::setFloatValue(const std::string& name, float value) {
		floatParameters[name] = value;
	}

	void GenericMaterialContainer::setIntValue(const std::string& name, int value) {
		intParameters[name] = value;
	}

	void GenericMaterialContainer::setVectorValue(const std::string& name, const glm::vec3& value) {
		vec3Parameters[name] = value;
	}

	void GenericMaterialContainer::setMatrixValue(const std::string& name, mat4 value) {
		mat4Parameters[name] = value;
	}

	void GenericMaterialContainer::iterTextures(std::function<void(const std::string&, const ITexture&)> function) const {
		for (auto it(textures.begin()); it != textures.end(); it++) {
			const std::string& name = it->first;
			const ITexture& texture = *it->second;
			function(name, texture);
		}
	}

	size_t GenericMaterialContainer::getTextureCount() const {
		return textures.size();
	}


}
