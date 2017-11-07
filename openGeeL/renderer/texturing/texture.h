#ifndef TEXTURE_H
#define TEXTURE_H

#include "texturetoken.h"
#include "texturetype.h"
#include "utility/resolution.h"

namespace geeL {

	class IFrameBuffer;
	class RenderShader;

	
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

		//Assing this texture to given framebuffer
		virtual void assignTo(const IFrameBuffer& buffer, unsigned int position, bool bindFB = false);

		//Assign this texture to given framebuffer besides an already assigned main buffer. 
		//Note: This function will only work if texture has been assigned to a main buffer beforehand
		virtual bool assignToo(const IFrameBuffer& buffer, unsigned int position, bool bindFB = false) const;

		//Bind texture as image texture
		void bindImage(unsigned int position = 0, AccessType access = AccessType::All) const;

		//Call GL disable function with appropriate texture type
		void disable() const;

		virtual void clear();
		virtual bool isEmpty() const;
		bool isAssigned() const;

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
		const IFrameBuffer* parent;

		Texture(ColorType colorType) : colorType(colorType), parent(nullptr) {}

	};
	

	class Texture2D : public Texture {

	public:
		virtual ~Texture2D() {}

		void mipmap() const;

		virtual void assignTo(const IFrameBuffer& buffer, unsigned int position, bool bindFB = false);
		virtual bool assignToo(const IFrameBuffer& buffer, unsigned int position, bool bindFB = false) const;

		virtual void initStorage(unsigned char* image);
		virtual void reserveStorage(unsigned int levels = 1);

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

	inline bool Texture::isAssigned() const {
		return parent != nullptr;
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
