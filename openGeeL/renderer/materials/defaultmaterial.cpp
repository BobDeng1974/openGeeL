#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "../shader/sceneshader.h"
#include "../texturing/imagetexture.h"
#include "defaultmaterial.h"

using namespace std;

namespace geeL {

	DefaultMaterialContainer::DefaultMaterialContainer() 
		: MaterialContainer("material"), transparency(1.f) {}


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

	float DefaultMaterialContainer::getTransparency() const {
		return transparency;
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

	void DefaultMaterialContainer::setTransparency(float value) {
		if (value != transparency && value >= 0.f && value <= 1.f)
			transparency = value;
	}

	void DefaultMaterialContainer::setRoughness(float value) {
		if (value != roughness && value >= 0.f && value <= 1.f)
			roughness = value;
	}

	void DefaultMaterialContainer::setMetallic(float value) {
		if (value != metallic && value >= 0.f && value <= 1.f)
			metallic = value;
	}

	void DefaultMaterialContainer::setColor(const glm::vec3& value) {
		color = value;
	}

	float DefaultMaterialContainer::getFloatValue(std::string name) const {
		if (name == "Transparency")
			return transparency;
		else if (name == "Roughness")
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
		if (name == "Transparency")
			setTransparency(value);
		else if (name == "Roughness")
			setRoughness(value);
		else if (name == "Metallic")
			setMetallic(value);
		else
			cout << "Value '" + name + "' not present in material\n";
	}

	void  DefaultMaterialContainer::setIntValue(std::string name, int value) {
		cout << "Value '" + name + "' not present in material\n";
	}

	void  DefaultMaterialContainer::setVectorValue(std::string name, const glm::vec3& value) {
		if (name == "Color")
			setColor(value);
		else 
			cout << "Value '" + name + "' not present in material\n";
	}


	void DefaultMaterialContainer::bindTextures(SceneShader& shader) const {
		shader.use();
		textureStack.bind(shader, "");
	}

	void DefaultMaterialContainer::bind(SceneShader& shader) const {
		shader.use();
		textureStack.draw(shader);

		shader.setInteger("material.mapFlags", textureStack.mapFlags);
		shader.setFloat("material.roughness", roughness);
		shader.setFloat("material.metallic", metallic);
		shader.setVector4("material.color", glm::vec4(color, transparency));
	}
}