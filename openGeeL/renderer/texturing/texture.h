#ifndef TEXTURE_H
#define TEXTURE_H

#include "texturetoken.h"
#include "texturetype.h"
#include "utility/resolution.h"

namespace geeL {

	class TextureParameters;

	
	class Texture {

	public:
		virtual ~Texture() {}

		virtual unsigned int getID() const;
		const TextureToken& getTextureToken() const;
		virtual TextureType getTextureType() const = 0;
		virtual ColorType getColorType() const;

		//Bind texture as texture sampler
		virtual void bind() const;
		void unbind() const;

		//Bind texture to given texture layer
		void bind(unsigned int layer) const;

		//Bind texture as image texture
		void bindImage(unsigned int position = 0, AccessType access = AccessType::All) const;

		//Call GL disable function with appropriate texture type
		void disable() const;

		virtual void clear();
		virtual bool isEmpty() const;

		virtual void initFilterMode(FilterMode mode);
		virtual void initWrapMode(WrapMode mode);
		virtual void initAnisotropyFilter(AnisotropicFilter filter);
		void setBorderColors(float r, float g, float b, float a);

		static void setMaxAnisotropyAmount(AnisotropicFilter value);
		static AnisotropicFilter getMaxAnisotropyAmount();

		void attachParameters(const TextureParameters& parameters);
		void detachParameters();

		bool operator== (const Texture& rhs) const;

	protected:
		TextureToken id;
		ColorType colorType;
		static AnisotropicFilter maxAnisotropy;
		const TextureParameters* parameters;
		
		Texture(ColorType colorType) : colorType(colorType), parameters(nullptr) {}

	};
	

	class Texture2D : public Texture {

	public:
		virtual ~Texture2D() {}

		void mipmap() const;

		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

		//Set render view to resolution of this texture
		virtual void setRenderResolution() const;
		virtual const Resolution& getResolution() const;
		ResolutionScale getScale() const;

		static void unbind();

	protected:
		Resolution resolution;

		Texture2D(ColorType colorType) : Texture(colorType) {}
		Texture2D(ColorType colorType, Resolution resolution) 
			: Texture(colorType), resolution(resolution) {}

		virtual void initStorage(unsigned char* image);
		virtual void reserveStorage(unsigned int levels = 1);

		void setResolution(const Resolution& resolution);
		void setResolution(unsigned int width, unsigned int height);

	};


	class Texture3D : public Texture {

	public:
		virtual ~Texture3D() {}

		void mipmap() const;

		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

		static void unbind();

	protected:
		Texture3D(ColorType colorType) : Texture(colorType) {}

	};


	class TextureCube : public Texture {

	public:
		virtual ~TextureCube() {}

		void mipmap() const;

		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

		virtual unsigned int getResolution() const = 0;

		static void unbind();

	protected:
		TextureCube(ColorType colorType) : Texture(colorType) {}

		virtual void initStorage(unsigned char* image);

	};


	



	inline unsigned int Texture::getID() const {
		return id;
	}

	inline const TextureToken & Texture::getTextureToken() const {
		return id;
	}

	inline ColorType Texture::getColorType() const {
		return colorType;
	}

	inline bool Texture::isEmpty() const {
		return getID() == 0;
	}

	inline bool Texture::operator== (const Texture& rhs) const {
		return getID() == rhs.getID();
	}

	inline const Resolution& Texture2D::getResolution() const {
		return resolution;
	}

	inline ResolutionScale Texture2D::getScale() const {
		return resolution.getScale();
	}

	inline void Texture2D::setResolution(const Resolution& resolution) {
		this->resolution = resolution;
	}

	inline void Texture2D::setResolution(unsigned int width, unsigned int height) {
		resolution = Resolution(width, height);
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
