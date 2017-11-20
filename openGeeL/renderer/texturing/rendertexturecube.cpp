#define GLEW_STATIC
#include <glew.h>
#include <cassert>
#include "rendertexturecube.h"

namespace geeL {

	RenderTextureCube::RenderTextureCube(unsigned int resolution,
		WrapMode wrapMode, FilterMode filterMode) : TextureCube(ColorType::RGB16), resolution(resolution) {

		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		for (unsigned int side = 0; side < 6; side++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, GL_RGB16F,
				resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);

		initFilterMode(filterMode);
		initWrapMode(wrapMode);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	RenderTextureCube::RenderTextureCube(const Texture& other, unsigned int resolution) 
		: TextureCube(other.getColorType()), resolution(resolution) {

		assert(other.getTextureType() == TextureType::TextureCube && "Texture has to be a texture cube");
		id = other.getTextureToken();
	}

}