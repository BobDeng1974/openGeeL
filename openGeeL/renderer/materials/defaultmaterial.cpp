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
		, transparency(1.f) 
		, translucency(1.f) {}


	void DefaultMaterialContainer::addTexture(const std::string& name, MemoryObject<ITexture> texture) {
		MapType type = MapTypeConversion::getMapFromString(name);
		addTexture(type, texture);
	}

	
	void DefaultMaterialContainer::addTexture(MemoryObject<TextureMap> texture) {
		addTexture(texture->type, texture);
	}

	void DefaultMaterialContainer::addTextures(std::vector<MemoryObject<TextureMap>> textures) {
		for (size_t i = 0; i < textures.size(); i++) {
			MemoryObject<TextureMap> texture = textures[i];
			addTexture(texture->type, texture);
		}
	}

	void DefaultMaterialContainer::addTexture(const MapType& type, MemoryObject<ITexture> texture) {
		assert(texture->getTextureType() == TextureType::Texture2D);

		//Interpret roughness map as inversed specular map	
		if (type == MapType::Roughness) {
			textureStack.addTexture("material.", texture, MapType::Gloss);
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
			case MapType::Gloss:
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

	float DefaultMaterialContainer::getTranslucency() const {
		return translucency;
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

	void DefaultMaterialContainer::setTranslucency(float value) {
		if (value != translucency && value >= 0.f && value <= 1.f)
			translucency = value;

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
		else if (name == "Metallic")
			return metallic;
		else if (name == "Translucency")
			return translucency;

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
		else if (name == "Translucency")
			setTranslucency(value);
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

	void DefaultMaterialContainer::iterTextures(std::function<void(const std::string&, const ITexture&)> function) const {
		const TextureStack& stack = textureStack;

		stack.iterTextures([&function](const std::string& name, const ITexture& texture) {
			function(name, texture);
		});
	}

	size_t DefaultMaterialContainer::getTextureCount() const {
		return textureStack.getTextureCount();
	}


	void DefaultMaterialContainer::bind(SceneShader& shader) const {
		textureStack.bindTextures(shader);

		shader.bind<int>("material.mapFlags", textureStack.mapFlags);
		shader.bind<int>("material.invSpec", inverseRoughness);
		shader.bind<float>("material.roughness", roughness);
		shader.bind<float>("material.metallic", metallic);
		shader.bind<float>("material.translucencyFactor", translucency);
		shader.bind<glm::vec4>("material.color", glm::vec4(color, transparency));
		shader.bind<glm::vec3>("material.emissivity", emissivity);
	}

}