#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include "texturetoken.h"
#include "texturetype.h"
#include "utility/resolution.h"

typedef unsigned int AttachmentPosition;
typedef unsigned int MipLevel;

namespace geeL {

	class TextureParameters;


	class ITexture {

	public:
		virtual ~ITexture() {}

		virtual TextureType getTextureType() const = 0;

		virtual void bind() const = 0;
		virtual void bind(unsigned int layer) const = 0;
		virtual void bindImage(unsigned int position = 0, AccessType access = AccessType::All) const = 0;

		virtual void unbind() const = 0;
		virtual void disable() const = 0;
		virtual bool isEmpty() const = 0;

		bool operator== (const ITexture& rhs) const;
		bool operator!= (const ITexture& rhs) const;

	protected:
		virtual unsigned int getID() const = 0;

	};
	

	class Texture : public ITexture {
		friend class FunctionalTexture;

	public:
		Texture(Texture&& other);
		virtual ~Texture() {}

		Texture& operator=(Texture&& other);

		
		const TextureToken& getTextureToken() const;
		
		virtual ColorType getColorType() const;

		//Bind texture as texture sampler
		virtual void bind() const;
		virtual void unbind() const;

		//Bind texture to given texture layer
		virtual void bind(unsigned int layer) const;

		//Bind texture as image texture
		virtual void bindImage(unsigned int position = 0, AccessType access = AccessType::All) const;

		//Assign this texture into given framebuffer attachment position
		virtual void assign(AttachmentPosition position, MipLevel level = 0) const = 0;
		virtual void assignDepth(MipLevel level = 0) const = 0;

		virtual void disable() const;
		virtual bool isEmpty() const;

		virtual void mipmap() const {}
		virtual void clear();

		virtual void initFilterMode(FilterMode mode);
		virtual void initWrapMode(WrapMode mode);
		virtual void initAnisotropyFilter(AnisotropicFilter filter);
		void setBorderColors(float r, float g, float b, float a);

		static void setMaxAnisotropyAmount(AnisotropicFilter value);
		static AnisotropicFilter getMaxAnisotropyAmount();

		virtual Resolution getScreenResolution() const = 0;
		virtual void setRenderResolution() const = 0;

		void attachParameters(const TextureParameters& parameters);
		void detachParameters();

	protected:
		static AnisotropicFilter maxAnisotropy;

		TextureToken id;
		ColorType colorType;
		const TextureParameters* parameters;
		
		Texture(ColorType colorType);

		virtual unsigned int getID() const;


	};
	

	class Texture2D : public Texture {

	public:
		Texture2D(ColorType colorType);
		Texture2D(Resolution resolution, ColorType colorType);
		Texture2D(Resolution resolution, ColorType colorType, void* image);

		Texture2D(Resolution resolution, ColorType colorType, FilterMode filterMode,
			WrapMode wrapMode, void* image);

		Texture2D(Resolution resolution, ColorType colorType, FilterMode filterMode,
			WrapMode wrapMode, AnisotropicFilter filter, void* image);

		Texture2D(Texture2D&& other);

		virtual ~Texture2D() {}


		Texture2D& operator=(Texture2D&& other);

		virtual void mipmap() const;
		virtual void assign(AttachmentPosition position, MipLevel level = 0) const;
		virtual void assignDepth(MipLevel level = 0) const;

		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

		virtual const Resolution& getResolution() const;
		virtual Resolution getScreenResolution() const;
		virtual void setRenderResolution() const;

		ResolutionScale getScale() const;

		static void unbind();

	protected:
		Resolution resolution;

		virtual void initStorage(void* image);
		virtual void reserveStorage(unsigned int levels = 1);

		void setResolution(const Resolution& resolution);
		void setResolution(unsigned int width, unsigned int height);

	};


	class Texture3D : public Texture {

	public:
		Texture3D();
		Texture3D(unsigned int width,
			unsigned int height,
			unsigned int depth,
			unsigned int levels,
			ColorType colorType,
			FilterMode filterMode,
			WrapMode wrapMode);

		Texture3D(unsigned int width,
			unsigned int height,
			unsigned int depth,
			unsigned int levels,
			ColorType colorType,
			FilterMode filterMode,
			WrapMode wrapMode, const std::vector<float>& buffer);

		Texture3D(Texture3D&& other);

		virtual ~Texture3D() {}


		Texture3D& operator=(Texture3D&& other);

		virtual void mipmap() const;
		virtual void assign(AttachmentPosition position, MipLevel level = 0) const;
		virtual void assignDepth(MipLevel level = 0) const;

		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

		virtual Resolution getScreenResolution() const;
		virtual void setRenderResolution() const;

		static void unbind();

	protected:
		unsigned int width, height, depth, levels;

		void initStorage();
		void initStorage(const std::vector<float>& buffer);

	};


	class TextureCube : public Texture {

	public:
		TextureCube(ColorType colorType);
		TextureCube(unsigned int resolution, ColorType colorType);

		TextureCube(unsigned int resolution, ColorType colorType,
			FilterMode filterMode, WrapMode wrapMode);

		TextureCube(unsigned int resolution, ColorType colorType,
			FilterMode filterMode, WrapMode wrapMode, unsigned char* images[6]);

		TextureCube(TextureCube&& other);

		virtual ~TextureCube() {}


		TextureCube& operator=(TextureCube&& other);

		virtual void mipmap() const;
		virtual void assign(AttachmentPosition position, MipLevel level = 0) const;
		virtual void assignSide(AttachmentPosition position, MipLevel level, unsigned int side) const;
		virtual void assignDepth(MipLevel level = 0) const;

		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

		virtual unsigned int getResolution() const;
		virtual Resolution getScreenResolution() const;
		virtual void setRenderResolution() const;

		static void unbind();

	protected:
		virtual void initStorage(unsigned char* image);
		void initStorage(unsigned char* images[6]);

	private:
		unsigned int resolution;

	};


	inline bool ITexture::isEmpty() const {
		return getID() == 0;
	}

	inline bool ITexture::operator==(const ITexture& rhs) const {
		return getID() == rhs.getID();
	}

	inline bool ITexture::operator!=(const ITexture& rhs) const {
		return getID() != rhs.getID();
	}

	inline unsigned int Texture::getID() const {
		return id;
	}

	inline const TextureToken& Texture::getTextureToken() const {
		return id;
	}

	inline ColorType Texture::getColorType() const {
		return colorType;
	}

	inline const Resolution& Texture2D::getResolution() const {
		return resolution;
	}

	inline Resolution Texture2D::getScreenResolution() const {
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

	inline Resolution Texture3D::getScreenResolution() const {
		return Resolution(width, height);
	}

	inline TextureType TextureCube::getTextureType() const {
		return TextureType::TextureCube;
	}

	inline Resolution TextureCube::getScreenResolution() const {
		unsigned int res = getResolution();
		return Resolution(res);
	}

	inline unsigned int TextureCube::getResolution() const {
		return resolution;
	}

}

#endif
