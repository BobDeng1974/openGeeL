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

		FunctionalTexture(FunctionalTexture&& other);
		FunctionalTexture& operator=(FunctionalTexture&& other);

		
		virtual TextureType getTextureType() const;

		
		virtual void bind(unsigned int layer) const;
		virtual void bindImage(unsigned int position = 0, AccessType access = AccessType::All) const;

		virtual void unbind() const;
		virtual void disable() const;
		virtual bool isEmpty() const;

		const Texture& getTexture() const;

	protected:
		Texture& getTexture();
		void dereferenceTexture(bool del);

		virtual unsigned int getID() const;

	private:
		Texture* texture;

		FunctionalTexture(const FunctionalTexture& other) = delete;
		FunctionalTexture& operator=(const FunctionalTexture& other) = delete;

	};


	inline FunctionalTexture::FunctionalTexture(std::unique_ptr<Texture> innerTexture) 
		: texture(innerTexture.release()) {}

	inline FunctionalTexture::~FunctionalTexture() {
		if(texture != nullptr)
			delete texture;
	}

	inline FunctionalTexture::FunctionalTexture(FunctionalTexture&& other)
		: texture(other.texture) {

		other.texture = nullptr;
	}

	inline FunctionalTexture& FunctionalTexture::operator=(FunctionalTexture&& other) {
		if (this != &other) {
			texture = other.texture;
			other.texture = nullptr;
		}

		return *this;
	}


	inline unsigned int FunctionalTexture::getID() const {
		return texture->getID();
	}

	inline TextureType FunctionalTexture::getTextureType() const {
		return texture->getTextureType();
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

	inline bool FunctionalTexture::isEmpty() const {
		return texture->isEmpty();
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
