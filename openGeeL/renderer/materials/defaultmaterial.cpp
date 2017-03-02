#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../shader/shader.h"
#include "../texturing/simpletexture.h"
#include "defaultmaterial.h"

using namespace std;

namespace geeL {

	DefaultMaterialContainer::DefaultMaterialContainer(MaterialType type) 
		: MaterialContainer("material", type) {
		
		textureStack = LayeredTexture();
	}


	void DefaultMaterialContainer::addTexture(std::string name, SimpleTexture& texture) {
		textureStack.addTexture(this->name + "." + name, texture);
	}

	void DefaultMaterialContainer::addTextures(std::vector<SimpleTexture*> textures) {
		for (size_t i = 0; i < textures.size(); i++) {
			SimpleTexture& texture = *textures[i];
			string name = this->name + "." + texture.GetTypeAsString();

			textureStack.addTexture(name, texture);
		}
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