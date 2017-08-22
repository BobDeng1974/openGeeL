#define GLEW_STATIC
#include <glew.h>
#include "rendertexture3D.h"

namespace geeL {

	RenderTexture3D::RenderTexture3D(unsigned int width, unsigned int height, unsigned int depth, unsigned int levels, 
		WrapMode wrapMode, FilterMode filterMode) : Texture3D(ColorType::RGBA) {
		
		//buffer = std::vector<float>(4 * width * height * depth, 0.f);

		glGenTextures(1, &id.token);
		glBindTexture(GL_TEXTURE_3D, id.token);

		initWrapMode(wrapMode);
		initFilterMode(filterMode);

		glTexStorage3D(GL_TEXTURE_3D, levels, GL_RGBA8, width, height, depth);
		//glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, width, height, depth, 0, GL_RGBA, GL_FLOAT, &buffer[0]);
		
		glGenerateMipmap(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, 0);
	}

}