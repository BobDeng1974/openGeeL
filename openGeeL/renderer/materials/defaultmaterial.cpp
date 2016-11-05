#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../shader/shader.h"
#include "../texturing/simpletexture.h"
#include "defaultmaterial.h"

using namespace std;

namespace geeL {

	DefaultMaterial::DefaultMaterial(Shader& defaultShader, MaterialType type) 
		: Material(defaultShader, "material", type) {
		
		textureStack = LayeredTexture();
	}


	void DefaultMaterial::addTexture(std::string name, SimpleTexture& texture) {
		textureStack.addTexture(this->name + "." + name, texture);
	}

	void DefaultMaterial::addTextures(std::vector<SimpleTexture*> textures) {
		for (size_t i = 0; i < textures.size(); i++) {
			SimpleTexture& texture = *textures[i];
			string name = this->name + "." + texture.GetTypeAsString();

			textureStack.addTexture(name, texture);
		}
	}

	void DefaultMaterial::setShininess(float value) {
		shininess = value;
	}

	void DefaultMaterial::bindTextures() const {
		shader.use();
		textureStack.bind(shader, "");
	}

	void DefaultMaterial::bind() const {
		shader.use();
		shader.loadMaps();
		textureStack.draw(shader);

		shader.setInteger("material.mapFlags", textureStack.mapFlags);
		shader.setFloat("material.type", type);
		shader.setFloat("material.shininess", shininess);
	}
}