#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include <vector>
#include "texture.h"

namespace geeL {

	class Texture3D : public Texture {

	public:
		Texture3D() : Texture(ColorType::Single), id(0) {}
		Texture3D(unsigned int width, unsigned int height, unsigned int depth, unsigned int levels, 
			WrapMode wrapMode = WrapMode::ClampBorder, FilterMode filterMode = FilterMode::Trilinear);

		virtual unsigned int getID() const;
		virtual TextureType getTextureType() const;
		virtual void remove();
		virtual void initWrapMode(WrapMode mode);

		void mipmap() const;

	private:
		unsigned int id;
		std::vector<float> buffer;

	};


	inline unsigned int Texture3D::getID() const {
		return id;
	}

	inline TextureType Texture3D::getTextureType() const {
		return TextureType::Texture3D;
	}

}

#endif
