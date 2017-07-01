#ifndef TEXTURE_H
#define TEXTURE_H

#include "texturetype.h"

namespace geeL {

	class RenderShader;


	class Texture {

	public:
		virtual unsigned int getID() const = 0;
		virtual TextureType getTextureType() const = 0;
		virtual ColorType getColorType() const;
		
		//Remove texture from GPU memory
		virtual void remove();
		virtual void bind() const;
		virtual void clear();
		virtual bool isEmpty() const;

		virtual void initColorType(int width, int height, unsigned char* image);
		virtual void initFilterMode(FilterMode mode);
		virtual void initWrapMode(WrapMode mode);
		virtual void initAnisotropyFilter(AnisotropicFilter filter);

		static void clear(ColorType type, unsigned int id);
		static void mipmap(TextureType type, unsigned int id);
		static void setMaxAnisotropyAmount(AnisotropicFilter value);

		bool operator== (const Texture& rhs) const;

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


	class Texture3D : public Texture {

	public:
		void mipmap() const;

		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

	protected:
		Texture3D(ColorType colorType) : Texture(colorType) {}

	};


	class TextureCube : public Texture {

	public:
		void mipmap() const;

		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

		virtual unsigned int getResolution() const = 0;

	protected:
		TextureCube(ColorType colorType) : Texture(colorType) {}

	};



	inline ColorType Texture::getColorType() const {
		return colorType;
	}

	inline bool Texture::isEmpty() const {
		return getID() == 0;
	}

	inline bool Texture::operator== (const Texture& rhs) const {
		return getID() == rhs.getID();
	}

	inline TextureType Texture2D::getTextureType() const {
		return TextureType::Texture2D;
	}

	inline TextureType Texture3D::getTextureType() const {
		return TextureType::Texture3D;
	}

	inline TextureType TextureCube::getTextureType() const {
		return TextureType::TextureCube;
	}

	
}

#endif
