#define GLEW_STATIC
#include <glew.h>
#include "rendertexture3D.h"

namespace geeL {

	RenderTexture3D::RenderTexture3D()
		: Texture3D(ColorType::None, 0, 0, 0) {}

	RenderTexture3D::RenderTexture3D(unsigned int width,
		unsigned int height, 
		unsigned int depth, 
		unsigned int levels, 
		WrapMode wrapMode, 
		FilterMode filterMode) 
			: Texture3D(ColorType::RGBA, width, height, depth) {
		
		glBindTexture(GL_TEXTURE_3D, id);

		initWrapMode(wrapMode);
		initFilterMode(filterMode);

		glTexStorage3D(GL_TEXTURE_3D, levels, GL_RGBA8, width, height, depth);
		glGenerateMipmap(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, 0);
	}

}