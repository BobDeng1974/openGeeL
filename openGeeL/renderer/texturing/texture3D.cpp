#define GLEW_STATIC
#include <glew.h>
#include "texture3D.h"

namespace geeL {

	Texture3D::Texture3D(unsigned int width, unsigned int height, unsigned int depth, unsigned int levels, 
		WrapMode wrapMode, FilterMode filterMode) : Texture(ColorType::RGBA) {
		
		buffer = std::vector<float>((4 * width * height * depth, 0.f));

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_3D, id);

		initWrapMode(wrapMode);
		initFilterMode(filterMode);

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

	void Texture3D::initWrapMode(WrapMode mode) {
		switch (mode) {
			case WrapMode::Repeat:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
				break;
			case WrapMode::MirrorRepeat:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
				break;
			case WrapMode::ClampEdge:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				break;
			case WrapMode::ClampBorder:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
				break;
		}
	}

}