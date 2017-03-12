#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "../shader/shader.h"
#include "../texturing/simpletexture.h"
#include "defaultmaterial.h"

using namespace std;

namespace geeL {

	DefaultMaterialContainer::DefaultMaterialContainer(MaterialType type) 
		: MaterialContainer("material", type) {
		
		textureStack = LayeredTexture();
	}


	void DefaultMaterialContainer::addTexture(std::string name, TextureMap& texture) {
		textureStack.addTexture(this->name + "." + name, texture);
	}

	void DefaultMaterialContainer::addTextures(std::vector<TextureMap*> textures) {
		for (size_t i = 0; i < textures.size(); i++) {
			TextureMap& texture = *textures[i];
			string name = this->name + "." + texture.getTypeAsString();

			textureStack.addTexture(name, texture);
		}
	}

	float DefaultMaterialContainer::getRoughness() const {
		return roughness;
	}

	float DefaultMaterialContainer::getMetallic() const {
		return metallic;
	}

	glm::vec3 DefaultMaterialContainer::getColor() const {
		return color;
	}

	void DefaultMaterialContainer::setRoughness(float value) {
		roughness = value;
	}

	void DefaultMaterialContainer::setMetallic(float value) {
		metallic = value;
	}

	void DefaultMaterialContainer::setColor(glm::vec3 value) {
		color = value;
	}

	float DefaultMaterialContainer::getFloatValue(std::string name) const {
		if (name == "Roughness")
			return roughness;
		if (name == "Metallic")
			return metallic;

		cout << "Value '" + name + "' not present in material\n";
		return 0.f;
	}

	int DefaultMaterialContainer::getIntValue(std::string name) const {
		cout << "Value '" + name + "' not present in material\n";
		return 0;
	}

	glm::vec3 DefaultMaterialContainer::getVectorValue(std::string name) const {
		if (name == "Color")
			return color;

		cout << "Value '" + name + "' not present in material\n";
		return glm::vec3();
	}

	void  DefaultMaterialContainer::setFloatValue(std::string name, float value) {
		if (name == "Roughness")
			roughness = value;
		else if (name == "Metallic")
			metallic = value;
		else
			cout << "Value '" + name + "' not present in material\n";
	}

	void  DefaultMaterialContainer::setIntValue(std::string name, int value) {
		cout << "Value '" + name + "' not present in material\n";
	}

	void  DefaultMaterialContainer::setVectorValue(std::string name, const glm::vec3& value) {
		if (name == "Color")
			color = value;
		else 
			cout << "Value '" + name + "' not present in material\n";
	}


	void DefaultMaterialContainer::bindTextures(Shader& shader) const {
		shader.use();
		textureStack.bind(shader, "");
	}

	void DefaultMaterialContainer::bind(Shader& shader) const {
		shader.use();
		shader.loadMaps();
		textureStack.draw(shader);

		shader.setInteger("material.mapFlags", textureStack.mapFlags);
		shader.setFloat("material.type", type);
		shader.setFloat("material.roughness", roughness);
		shader.setFloat("material.metallic", metallic);
		shader.setVector3("material.color", color);
	}
}