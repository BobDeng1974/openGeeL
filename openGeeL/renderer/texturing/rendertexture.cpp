#define GLEW_STATIC
#include <glew.h>
#include "rendertexture.h"

namespace geeL {

	RenderTexture::RenderTexture(unsigned int width, unsigned int height, ColorType colorType, 
		WrapMode wrapMode, FilterMode filterMode) : width(width), height(height) {

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		Texture::initColorType(colorType, width, height, 0);
		Texture::initWrapMode(wrapMode);
		Texture::initFilterMode(filterMode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	unsigned int RenderTexture::getID() const {
		return id;
	}

	void RenderTexture::remove() {
		glDeleteTextures(1, &id);
	}

}