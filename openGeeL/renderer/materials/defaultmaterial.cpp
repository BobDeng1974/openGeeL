#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "shader/sceneshader.h"
#include "texturing/imagetexture.h"
#include "defaultmaterial.h"

using namespace std;

namespace geeL {

	DefaultMaterialContainer::DefaultMaterialContainer() 
		: MaterialContainer("material"), transparency(1.f) {}


	void DefaultMaterialContainer::addTexture(const std::string& name, Texture2D& texture) {
		MapType type = MapTypeConversion::getMapFromString(name);
		textureStack.addTexture("material.", texture, type);
	}

	void DefaultMaterialContainer::addTexture(TextureMap& texture) {
		textureStack.addTexture("material.", texture);
	}

	void DefaultMaterialContainer::addTextures(std::vector<TextureMap*> textures) {
		for (size_t i = 0; i < textures.size(); i++) {
			TextureMap& texture = *textures[i];
			string name = "material.";

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

	const glm::vec3& DefaultMaterialContainer::getColor() const {
		return color;
	}

	const glm::vec3& DefaultMaterialContainer::getEmissivity() const {
		return emissivity;
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

	void DefaultMaterialContainer::setEmissivity(const glm::vec3& value) {
		emissivity = value;
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
		else if (name == "Emissivity")
			return emissivity;

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
		if (name == "InverseRoughness")
			inverseRoughness = bool(value);
		else
			cout << "Value '" + name + "' not present in material\n";
	}

	void  DefaultMaterialContainer::setVectorValue(std::string name, const glm::vec3& value) {
		if (name == "Color")
			setColor(value);
		else if (name == "Emissivity")
			setEmissivity(value);
		else 
			cout << "Value '" + name + "' not present in material\n";
	}


	void DefaultMaterialContainer::bind(SceneShader& shader) const {
		textureStack.bind(shader);
		textureStack.draw(shader);

		shader.bind<int>("material.mapFlags", textureStack.mapFlags);
		shader.bind<int>("material.invSpec", inverseRoughness);
		shader.bind<float>("material.roughness", roughness);
		shader.bind<float>("material.metallic", metallic);
		shader.bind<glm::vec4>("material.color", glm::vec4(color, transparency));
		shader.bind<glm::vec3>("material.emissivity", emissivity);
	}

}