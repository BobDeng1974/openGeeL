#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>

namespace geeL {

class RenderShader;

	enum class ColorType {
		GammaSpace,
		Single,
		RGB,
		RGBA,
		RGB16,
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


	class Texture {

	public:
		virtual unsigned int getID() const = 0;

		//Remove texture from GPU memory
		virtual void remove() = 0;

		static void initColorType(ColorType type, int width, int height, unsigned char* image);
		static void initFilterMode(FilterMode mode);
		static void initWrapMode(WrapMode mode);
		static void initAnisotropyFilter(AnisotropicFilter filter);

		void mipmap() const;
		static void mipmap(unsigned int id);
		static void mipmapCube(unsigned int id);
		
		static void setMaxAnisotropyAmount(AnisotropicFilter value);

	private:
		static AnisotropicFilter maxAnisotropy;

	};

}

#endif
