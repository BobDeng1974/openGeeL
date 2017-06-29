#define GLEW_STATIC
#include <glew.h>
#include "rendertexture.h"

namespace geeL {

	RenderTexture::RenderTexture(const RenderTexture& texture) 
		: Texture2D(texture), id(texture.getID()), width(texture.width), height(texture.height) {}

	RenderTexture::RenderTexture(unsigned int width, unsigned int height, 
		ColorType colorType, WrapMode wrapMode, FilterMode filterMode) 
			: Texture2D(colorType), width(width), height(height) {

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		initColorType(width, height, 0);
		initWrapMode(wrapMode);
		initFilterMode(filterMode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int RenderTexture::getID() const {
		return id;
	}

	void RenderTexture::remove() {
		glDeleteTextures(1, &id);
	}

}