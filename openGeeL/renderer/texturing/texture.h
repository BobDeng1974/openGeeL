#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include "texturetoken.h"
#include "texturetype.h"
#include "utility/resolution.h"
#include "utility/functionguard.h"

typedef unsigned int AttachmentPosition;
typedef unsigned int MipLevel;

namespace geeL {

	class TextureParameters;


	class ITexture {

	public:
		virtual ~ITexture() {}

		virtual TextureType getTextureType() const = 0;

		virtual void bind(unsigned int layer) const = 0;
		virtual void bindImage(unsigned int position = 0, AccessType access = AccessType::All) const = 0;

		virtual void unbind() const = 0;
		virtual void disable() const = 0;
		virtual bool isEmpty() const = 0;

		bool operator== (const ITexture& rhs) const;
		bool operator!= (const ITexture& rhs) const;

		virtual unsigned int getID() const = 0;

	protected:
		virtual unsigned int getGPUID() const = 0;

	};
	

	class Texture : public ITexture {
		friend class FunctionalTexture;

	public:
		virtual ~Texture() {}

		Texture(Texture&& other);
		Texture& operator=(Texture&& other);

		
		//Bind texture to given texture layer
		virtual void bind(unsigned int layer) const;
		virtual void unbind() const;

		//Bind texture as image texture
		virtual void bindImage(unsigned int position = 0, AccessType access = AccessType::All) const;

		//Assign this texture into given framebuffer attachment position
		virtual void assign(AttachmentPosition position, MipLevel level = 0) const = 0;
		virtual void assignDepth(MipLevel level = 0) const = 0;

		virtual void disable() const;
		virtual bool isEmpty() const;

		virtual void mipmap() const {}
		virtual void clear();

		//Resize this texture to given resolution
		//Note: Internal image data will be lost
		virtual void resize(Resolution resolution) = 0;

		virtual void initFilterMode(FilterMode mode);
		virtual void initWrapMode(WrapMode mode);
		virtual void initAnisotropyFilter(AnisotropicFilter filter);
		void setBorderColors(float r, float g, float b, float a);

		const TextureToken& getTextureToken() const;
		virtual ColorType getColorType() const;

		static void setMaxAnisotropyAmount(AnisotropicFilter value);
		static AnisotropicFilter getMaxAnisotropyAmount();

		virtual Resolution getScreenResolution() const = 0;
		virtual void applyRenderResolution() const = 0;

		void attachParameters(const TextureParameters& parameters);
		void detachParameters();

		//Calling texture like this ensures automatic binding
		//and unbinding of underlying GPU data
		FunctionGuard<Texture> operator->();

		virtual unsigned int getID() const;

	protected:
		static AnisotropicFilter maxAnisotropy;
		const TextureParameters* parameters;

		unsigned int id;
		TextureToken gid;
		ColorType colorType;
		FilterMode filterMode;
		WrapMode wrapMode; 
		AnisotropicFilter filter;
		
		Texture(ColorType colorType, 
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat, 
			AnisotropicFilter filter = AnisotropicFilter::None);

		virtual void bind() const;
		virtual unsigned int getGPUID() const;

	};
	

	class Texture2D : public Texture {

	public:
		Texture2D(ColorType colorType);
		Texture2D(Resolution resolution, ColorType colorType, FilterMode filterMode,
			WrapMode wrapMode, AnisotropicFilter filter, void* image = 0);
		virtual ~Texture2D() {}

		Texture2D(Texture2D&& other);
		Texture2D& operator=(Texture2D&& other);


		virtual void assign(AttachmentPosition position, MipLevel level = 0) const;
		virtual void assignDepth(MipLevel level = 0) const;

		virtual void mipmap() const;
		virtual void resize(Resolution resolution);

		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

		virtual const Resolution& getResolution() const;
		virtual Resolution getScreenResolution() const;
		virtual void applyRenderResolution() const;

		ResolutionScale getScale() const;

		static void unbind();

	protected:
		Resolution resolution;

		virtual void initStorage(void* image);
		virtual void reserveStorage(unsigned int levels = 1);

		void setResolution(const Resolution& resolution);

	};


	class Texture3D : public Texture {

	public:
		Texture3D(unsigned int width,
			unsigned int height,
			unsigned int depth,
			unsigned int levels,
			ColorType colorType,
			FilterMode filterMode,
			WrapMode wrapMode, 
			float* buffer = 0);
		virtual ~Texture3D() {}

		Texture3D(Texture3D&& other);
		Texture3D& operator=(Texture3D&& other);


		virtual void assign(AttachmentPosition position, MipLevel level = 0) const;
		virtual void assignDepth(MipLevel level = 0) const;

		virtual void mipmap() const;
		virtual void resize(Resolution resolution);

		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

		virtual Resolution getScreenResolution() const;
		virtual void applyRenderResolution() const;

		static void unbind();

	protected:
		unsigned int width, height, depth, levels;

		void initStorage(float* buffer);

	};


	class TextureCube : public Texture {

	public:
		TextureCube(unsigned int resolution, ColorType colorType,
			FilterMode filterMode, WrapMode wrapMode, void* images[6] = 0);
		virtual ~TextureCube() {}

		TextureCube(TextureCube&& other);
		TextureCube& operator=(TextureCube&& other);

		
		virtual void assign(AttachmentPosition position, MipLevel level = 0) const;
		virtual void assignSide(AttachmentPosition position, MipLevel level, unsigned int side) const;
		virtual void assignDepth(MipLevel level = 0) const;

		virtual void mipmap() const;
		virtual void resize(Resolution resolution);

		virtual void initWrapMode(WrapMode mode);
		virtual TextureType getTextureType() const;

		virtual unsigned int getResolution() const;
		virtual Resolution getScreenResolution() const;
		virtual void applyRenderResolution() const;

		static void unbind();

	protected:
		void initStorage(void* images[6]);

	private:
		unsigned int resolution;

	};


	inline bool ITexture::isEmpty() const {
		return getGPUID() == 0;
	}

	inline bool ITexture::operator==(const ITexture& rhs) const {
		return getGPUID() == rhs.getGPUID();
	}

	inline bool ITexture::operator!=(const ITexture& rhs) const {
		return getGPUID() != rhs.getGPUID();
	}

	inline unsigned int Texture::getGPUID() const {
		return gid;
	}

	inline const TextureToken& Texture::getTextureToken() const {
		return gid;
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
