#include "rendertexture3D.h"

namespace geeL {

	RenderTexture3D::RenderTexture3D()
		: Texture3D() {}

	RenderTexture3D::RenderTexture3D(unsigned int width,
		unsigned int height, 
		unsigned int depth, 
		unsigned int levels, 
		ColorType colorType,
		WrapMode wrapMode, 
		FilterMode filterMode) 
			: Texture3D(width, height, depth, levels, 
				colorType, filterMode, wrapMode) {
		
		mipmap();
	}

}