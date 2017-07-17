#define GLEW_STATIC
#include <glew.h>
#include "rendertexture.h"
#include <iostream>

namespace geeL {

	RenderTexture::RenderTexture(Resolution resolution,
		ColorType colorType, WrapMode wrapMode, FilterMode filterMode) 
			: Texture2D(colorType, resolution) {

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		initColorType(resolution.getWidth(), resolution.getHeight(), 0);
		initWrapMode(wrapMode);
		initFilterMode(filterMode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	RenderTexture::~RenderTexture() {
		remove();
	}

	unsigned int RenderTexture::getID() const {
		return id;
	}

	void RenderTexture::remove() {
		glDeleteTextures(1, &id);
	}

	void RenderTexture::resize(Resolution resolution) {
		glBindTexture(GL_TEXTURE_2D, id);

		initColorType(resolution.getWidth(), resolution.getHeight(), 0);
		setResolution(resolution);
		
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}