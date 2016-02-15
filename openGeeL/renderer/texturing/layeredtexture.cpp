#define GLEW_STATIC
#include <glew.h>
#include "layeredtexture.h"
#include "simpletexture.h"

namespace geeL {

	void LayeredTexture::addTexture(const SimpleTexture& texture) {
		textures.push_back(texture);
	}

	void LayeredTexture::draw(GLint shader, int texLayer) {
		int layer = GL_TEXTURE0 + texLayer;

		for (size_t i = 0; i < textures.size(); i++) {
			SimpleTexture& texture = textures[i];
			glActiveTexture(layer);
			texture.draw(shader, i);

			layer++;
		}
	}

}