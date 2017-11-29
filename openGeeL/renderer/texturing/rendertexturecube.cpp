#define GLEW_STATIC
#include <glew.h>
#include <cassert>
#include "rendertexturecube.h"

namespace geeL {

	RenderTextureCube::RenderTextureCube(unsigned int resolution,
		WrapMode wrapMode, FilterMode filterMode) : TextureCube(ColorType::RGB16), resolution(resolution) {

		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		initStorage(0);

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