#define GLEW_STATIC
#include <glew.h>
#include "texture3D.h"

namespace geeL {

	Texture3D::Texture3D(unsigned int width, unsigned int height, unsigned int depth, unsigned int levels) {
		buffer = std::vector<float>((4 * width * height * depth, 0.f));

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_3D, id);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexStorage3D(GL_TEXTURE_3D, levels, GL_RGBA8, width, height, depth);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, width, height, depth, 0, GL_RGBA, GL_FLOAT, &buffer[0]);
		glGenerateMipmap(GL_TEXTURE_3D);
		
		glBindTexture(GL_TEXTURE_3D, 0);
	}


	void Texture3D::remove() {
		glDeleteTextures(1, &id);
	}

	void Texture3D::mipmap() const {
		glBindTexture(GL_TEXTURE_3D, id);
		glGenerateMipmap(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, 0);
	}

}