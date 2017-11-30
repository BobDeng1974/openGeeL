#include <cassert>
#include "rendertexturecube.h"

namespace geeL {

	RenderTextureCube::RenderTextureCube(unsigned int resolution, 
		ColorType colorType,
		WrapMode wrapMode, 
		FilterMode filterMode) 
			: TextureCube(resolution, 
				colorType, 
				filterMode, 
				wrapMode) {}

	RenderTextureCube::RenderTextureCube(const Texture& other, unsigned int resolution) 
		: TextureCube(other.getColorType()) {

		assert(other.getTextureType() == TextureType::TextureCube && "Texture has to be a texture cube");
		id = other.getTextureToken();
	}

}