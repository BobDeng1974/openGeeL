#ifndef TEXTURETYPE_H
#define TEXTURETYPE_H

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_CUBE_MAP 0x8513

namespace geeL {

	enum class TextureType {
		Texture2D = GL_TEXTURE_2D,
		Texture3D = GL_TEXTURE_3D,
		TextureCube = GL_TEXTURE_CUBE_MAP

	};
}

#endif
