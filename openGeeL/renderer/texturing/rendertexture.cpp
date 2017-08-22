#define GLEW_STATIC
#include <glew.h>
#include <cassert>
#include "rendertexture.h"

namespace geeL {

	RenderTexture::RenderTexture(Resolution resolution,
		ColorType colorType, WrapMode wrapMode, FilterMode filterMode) 
			: Texture2D(colorType, resolution) {

		glBindTexture(GL_TEXTURE_2D, id);
		initColorType(resolution.getWidth(), resolution.getHeight(), 0);
		initWrapMode(wrapMode);
		initFilterMode(filterMode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	RenderTexture::RenderTexture(const Texture& other, Resolution resolution) 
		: Texture2D(other.getColorType(), resolution) {

		assert(other.getTextureType() == TextureType::Texture2D, "Texture has to be a 2D texture");
		id = other.getTextureToken();
	}


	void RenderTexture::resize(Resolution resolution) {
		glBindTexture(GL_TEXTURE_2D, id);

		initColorType(resolution.getWidth(), resolution.getHeight(), 0);
		setResolution(resolution);
		
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}