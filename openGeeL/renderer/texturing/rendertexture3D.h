#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include <vector>
#include "texture.h"

namespace geeL {

	//3D Texture that is intended to be rendered into
	class RenderTexture3D : public Texture3D {

	public:
		RenderTexture3D() : Texture3D(ColorType::None) {}
		RenderTexture3D(unsigned int width, unsigned int height, unsigned int depth, unsigned int levels, 
			WrapMode wrapMode = WrapMode::ClampBorder, FilterMode filterMode = FilterMode::Trilinear);

	private:
		std::vector<float> buffer;

		RenderTexture3D(const RenderTexture3D& other) = delete;
		RenderTexture3D& operator= (const RenderTexture3D& other) = delete;

	};


}

#endif
