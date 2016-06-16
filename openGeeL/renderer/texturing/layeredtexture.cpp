#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "simpletexture.h"
#include "layeredtexture.h"

namespace geeL {

	void LayeredTexture::addTexture(string name, SimpleTexture& texture) {
		textures.push_back(pair<string, SimpleTexture*>(name, &texture));
	}

	void LayeredTexture::bind(const Shader& shader, const char* name, int texLayer) const {
		int layer = GL_TEXTURE0 + texLayer;

		for (size_t i = 0; i < textures.size(); i++) {
			string name = textures[i].first;
			SimpleTexture* texture = textures[i].second;

			texture->bind(shader, name.c_str(), i);
		}
	}

	void LayeredTexture::draw(int texLayer) const {
		int layer = GL_TEXTURE0 + texLayer;

		for (size_t i = 0; i < textures.size(); i++) {
			SimpleTexture* texture = textures[i].second;
			glActiveTexture(layer + i);

			texture->draw();
		}
	}

}