#ifndef TEXTURECUBE_H
#define TEXTURECUBE_H

#include "texture.h"

namespace geeL {

	//3D Texture that is intended to be rendered into
	class RenderTextureCube : public TextureCube {

	public:
		RenderTextureCube() : TextureCube(ColorType::Single) {}
		RenderTextureCube(unsigned int resolution, 
			ColorType colorType = ColorType::RGB16, 
			WrapMode wrapMode = WrapMode::ClampEdge,
			FilterMode filterMode = FilterMode::Linear);

		RenderTextureCube(const Texture& other, unsigned int resolution);


		virtual unsigned int getResolution() const;

	private:
		unsigned int resolution;

		RenderTextureCube(const RenderTextureCube& other) = delete;
		RenderTextureCube& operator= (const RenderTextureCube& other) = delete;

	};


	inline unsigned int RenderTextureCube::getResolution() const {
		return resolution;
	}
	
}

#endif
