#ifndef TEXTURE_H
#define TEXTURE_H

#include <cassert>
#include "texturetype.h"
#include "utility/resolution.h"

namespace geeL {

	class RenderShader;

	//Wrapper for texture id that manages GPU memory automatically
	class TextureToken {

	public:
		unsigned int token;

		TextureToken();
		TextureToken(const TextureToken& other);
		TextureToken(unsigned int token);
		~TextureToken();

		bool operator== (const TextureToken& other) const;
		bool operator!= (const TextureToken& other) const;

		bool operator== (unsigned int other) const;
		bool operator!= (unsigned int other) const;

	private:
		bool external;

		TextureToken& operator= (const TextureToken& other) = delete;

	};


	class Texture {

	public:
		virtual ~Texture() {}

		unsigned int getID() const;
		virtual TextureType getTextureType() const = 0;
		virtual ColorType getColorType() const;

		//Bind texture as texture sampler
		virtual void bind() const;
		void unbind() const;

		//Bind texture as image texture
		void bindImage(unsigned int position = 0, AccessType access = AccessType::All) const;

		//Call GL disable function with appropriate texture type
		void disable() const;

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
		TextureToken id;
		ColorType colorType;
		static AnisotropicFilter maxAnisotropy;

		Texture(ColorType colorType) : colorType(colorType) {}

	};
	

	class Texture2D : public Texture {

	public:
		virtual ~Texture2D() {}

		void mipmap() const;
		
		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

		//Set render view to resolution of this texture
		void setRenderResolution() const;
		const Resolution& getResolution() const;

		static void unbind();

	protected:
		Resolution resolution;

		Texture2D(ColorType colorType) : Texture(colorType) {}
		Texture2D(ColorType colorType, Resolution resolution) 
			: Texture(colorType), resolution(resolution) {}

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

	};


	inline TextureToken::TextureToken() : token(0), external(false) {}

	inline TextureToken::TextureToken(const TextureToken& other) : token(other.token), external(true) {
		assert(other.token != 0);
	}

	inline TextureToken::TextureToken(unsigned int token) : token(token), external(true) {
		assert(token != 0);
	}

	inline bool TextureToken::operator== (const TextureToken& other) const {
		return token == other.token;
	}

	inline bool TextureToken::operator!= (const TextureToken& other) const {
		return token != other.token;
	}

	inline bool TextureToken::operator== (unsigned int other) const {
		return token == other;
	}
	inline bool TextureToken::operator!= (unsigned int other) const {
		return token != other;
	}


	inline unsigned int Texture::getID() const {
		return id.token;
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
