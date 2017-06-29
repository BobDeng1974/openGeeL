#ifndef TEXTURE_H
#define TEXTURE_H

#include "texturetype.h"

namespace geeL {

	class RenderShader;

	enum class ColorType {
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


	class Texture {

	public:
		virtual unsigned int getID() const = 0;
		virtual TextureType getTextureType() const = 0;
		
		//Remove texture from GPU memory
		virtual void remove() = 0;
		virtual void bind() const;
		virtual void clear();
		virtual bool isEmpty() const;

		virtual void initColorType(int width, int height, unsigned char* image);
		virtual void initFilterMode(FilterMode mode);
		virtual void initWrapMode(WrapMode mode) = 0;
		virtual void initAnisotropyFilter(AnisotropicFilter filter);

		static void clear(ColorType type, unsigned int id);
		static void mipmap(TextureType type, unsigned int id);
		static void setMaxAnisotropyAmount(AnisotropicFilter value);

	protected:
		Texture(ColorType colorType) : colorType(colorType) {}

		ColorType colorType;
		static AnisotropicFilter maxAnisotropy;

	};


	class Texture2D : public Texture {

	public:
		void mipmap() const;
		
		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

	protected:
		Texture2D(ColorType colorType) : Texture(colorType) {}

	};


	inline bool Texture::isEmpty() const {
		return getID() == 0;
	}

	inline TextureType Texture2D::getTextureType() const {
		return TextureType::Texture2D;
	}

}

#endif
