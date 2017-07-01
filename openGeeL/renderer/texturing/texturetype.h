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

	enum class ColorType {
		None,
		GammaSpace,
		Single,
		RGB,
		RGB16,
		RGB32,
		RGBA,
		RGBA16,
		RGBA32
	};

	enum class FilterMode {
		None,
		Nearest,
		Linear,
		Bilinear,
		Trilinear
	};

	enum class WrapMode {
		Repeat,
		MirrorRepeat,
		ClampEdge,
		ClampBorder
	};

	enum class AnisotropicFilter {
		None = 0,
		Small = 2,
		Medium = 4,
		Large = 8,
		VeryLarge = 16
	};

}

#endif
