#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>

namespace geeL {

class Shader;

	enum class ColorType {
		GammaSpace,
		Single,
		RGB,
		RGBA,
		RGB16,
		RGBA16
	};

	enum class FilterMode {
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


	class Texture {

	public:
		virtual unsigned int getID() const = 0;
		virtual void remove() = 0;

		static void initColorType(ColorType type, int width, int height, unsigned char* image);
		static void initFilterMode(FilterMode mode);
		static void initWrapMode(WrapMode mode);
		
	};

}

#endif
