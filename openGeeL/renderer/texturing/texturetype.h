#ifndef TEXTURETYPE_H
#define TEXTURETYPE_H

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_CUBE_MAP 0x8513

#define GL_SRGB_ALPHA 0x8C42
#define GL_ALPHA 0x1906
#define GL_RED 0x1903
#define GL_R16F 0x822D
#define GL_RG16F 0x822F
#define GL_RGB 0x1907
#define GL_RGB16F 0x881B
#define GL_RGB32F 0x8815
#define GL_RGBA 0x1908
#define GL_RGBA8 0x8058
#define GL_RGBA16F 0x881A
#define GL_RGBA32F 0x8814
#define GL_DEPTH_COMPONENT 0x1902
#define GL_DEPTH_COMPONENT32 0x81A7

#define GL_READ_ONLY 0x88B8
#define GL_WRITE_ONLY 0x88B9
#define GL_READ_WRITE 0x88BA

namespace geeL {

	enum class TextureType {
		Texture2D = GL_TEXTURE_2D,
		Texture3D = GL_TEXTURE_3D,
		TextureCube = GL_TEXTURE_CUBE_MAP

	};

	enum class ColorType {
		None = 0,
		Depth = GL_DEPTH_COMPONENT,
		Depth32 = GL_DEPTH_COMPONENT32,
		GammaSpace = GL_SRGB_ALPHA,
		Single = GL_RED,
		Single16 = GL_R16F,
		RG16 = GL_RG16F,
		RGB = GL_RGB,
		RGB16 = GL_RGB16F,
		RGB32 = GL_RGB32F,
		RGBA = GL_RGBA,
		RGBA8 = GL_RGBA8,
		RGBA16 = GL_RGBA16F,
		RGBA32 = GL_RGBA32F
	};

	enum class FilterMode {
		None,
		Nearest,
		Linear,
		LinearMip,
		Bilinear,
		Trilinear,
		TrilinearUltra
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

	enum class AccessType {
		Read = GL_READ_ONLY,
		Write = GL_WRITE_ONLY,
		All = GL_READ_WRITE
	};

}

#endif
