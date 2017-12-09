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
		: MaterialContainer("material")
		, transparency(1.f) {}


	void DefaultMaterialContainer::addTexture(const std::string& name, std::shared_ptr<Texture2D> texture) {
		MapType type = MapTypeConversion::getMapFromString(name);
		addTexture(type, texture);
	}

	
	void DefaultMaterialContainer::addTexture(std::shared_ptr<TextureMap> texture) {
		addTexture(texture->type, texture);
	}

	void DefaultMaterialContainer::addTextures(std::vector<std::shared_ptr<TextureMap>> textures) {
		for (size_t i = 0; i < textures.size(); i++) {
			std::shared_ptr<TextureMap> texture = textures[i];
			addTexture(texture->type, texture);
		}
	}

	void DefaultMaterialContainer::addTexture(const MapType& type, std::shared_ptr<Texture2D> texture) {
		
		//Interpret roughness map as inversed specular map	
		if (type == MapType::Roughness) {
			textureStack.addTexture("material.", texture, MapType::Specular);
			inverseRoughness = true;
		}
		else
			textureStack.addTexture("material.", texture, type);

		//Reset corresponding property to default value 
		//(e.g. roughness to 1 if specular map is added)
		switch (type) {
			case MapType::Diffuse:
				setColor(glm::vec3(1.f));
				break;
			case MapType::Specular:
				setRoughness(1.f);
				break;
			case MapType::Metallic:
				setMetallic(1.f);
				break;
			case MapType::Emission:
				setEmissivity(glm::vec3(1.f));
				break;
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

	float DefaultMaterialContainer::getFloatValue(const std::string& name) const {
		if (name == "Transparency")
			return transparency;
		else if (name == "Roughness")
			return roughness;
		if (name == "Metallic")
			return metallic;

		cout << "Value '" + name + "' not present in material\n";
		return 0.f;
	}

	int DefaultMaterialContainer::getIntValue(const std::string& name) const {
		cout << "Value '" + name + "' not present in material\n";
		return 0;
	}

	glm::vec3 DefaultMaterialContainer::getVectorValue(const std::string& name) const {
		if (name == "Color")
			return color;
		else if (name == "Emissivity")
			return emissivity;

		cout << "Value '" + name + "' not present in material\n";
		return glm::vec3();
	}

	void  DefaultMaterialContainer::setFloatValue(const std::string& name, float value) {
		if (name == "Transparency")
			setTransparency(value);
		else if (name == "Roughness")
			setRoughness(value);
		else if (name == "Metallic")
			setMetallic(value);
		else
			cout << "Value '" + name + "' not present in material\n";
	}

	void  DefaultMaterialContainer::setIntValue(const std::string& name, int value) {
		if (name == "InverseRoughness")
			inverseRoughness = bool(value);
		else
			cout << "Value '" + name + "' not present in material\n";
	}

	void  DefaultMaterialContainer::setVectorValue(const std::string& name, const glm::vec3& value) {
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