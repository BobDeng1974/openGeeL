#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include <vector>
#include "texture.h"

namespace geeL {

	//3D Texture that is intended to be rendered into
	class RenderTexture3D : public Texture3D {

	public:
		RenderTexture3D() : Texture3D(ColorType::Single), id(0) {}
		RenderTexture3D(unsigned int width, unsigned int height, unsigned int depth, unsigned int levels, 
			WrapMode wrapMode = WrapMode::ClampBorder, FilterMode filterMode = FilterMode::Trilinear);

		virtual unsigned int getID() const;
		virtual void remove();

	private:
		unsigned int id;
		std::vector<float> buffer;

	};


	inline unsigned int RenderTexture3D::getID() const {
		return id;
	}

}

#endif
