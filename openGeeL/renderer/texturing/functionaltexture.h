#ifndef FUNCTIONALTEXTURE
#define FUNCTIONALTEXTURE

#include <memory>
#include "texture.h"

namespace geeL {


	//Base class for all textures whose functionality is independent from specific
	//texture type. Serves as wrapper around an actual (hardware) texture
	class FunctionalTexture : public ITexture {

	public:
		FunctionalTexture(std::unique_ptr<Texture> innerTexture);
		virtual ~FunctionalTexture();


		virtual unsigned int getID() const;
		virtual TextureType getTextureType() const;

		virtual void bind() const;
		virtual void bind(unsigned int layer) const;
		virtual void bindImage(unsigned int position = 0, AccessType access = AccessType::All) const;

		virtual void unbind() const;
		virtual void disable() const;

		const Texture& getTexture() const;

	protected:
		Texture& getTexture();
		void dereferenceTexture(bool del);

	private:
		Texture* texture;

	};


	inline FunctionalTexture::FunctionalTexture(std::unique_ptr<Texture> innerTexture) 
		: texture(innerTexture.release()) {}

	inline FunctionalTexture::~FunctionalTexture() {
		if(texture != nullptr)
			delete texture;
	}


	inline unsigned int FunctionalTexture::getID() const {
		return texture->getID();
	}

	inline TextureType FunctionalTexture::getTextureType() const {
		return texture->getTextureType();
	}

	inline void FunctionalTexture::bind() const {
		texture->bind();
	}

	inline void FunctionalTexture::bind(unsigned int layer) const {
		texture->bind(layer);
	}

	inline void FunctionalTexture::bindImage(unsigned int position, AccessType access) const {
		texture->bindImage(position, access);
	}

	inline void FunctionalTexture::unbind() const {
		texture->unbind();
	}

	inline void FunctionalTexture::disable() const {
		texture->disable();
	}

	inline const Texture& FunctionalTexture::getTexture() const {
		return *texture;
	}

	inline Texture& FunctionalTexture::getTexture() {
		return *texture;
	}

	inline void FunctionalTexture::dereferenceTexture(bool del) {
		if (del) delete texture;

		texture = nullptr;
	}

}


#endif
