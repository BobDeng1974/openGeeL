#define GLEW_STATIC
#include <glew.h>
#include <limits.h>
#include "texture.h"

namespace geeL {

	AnisotropicFilter Texture::maxAnisotropy;


	void Texture::clear(ColorType type, unsigned int id) {
		switch (type) {
			case ColorType::Single:
			case ColorType::RGB:
			case ColorType::RGB16: {
				float colors[3] = { 0, 0, 0 };
				glClearTexImage(id, 0, GL_RGB, GL_FLOAT, &colors);
				break;
			}
			case ColorType::GammaSpace:
			case ColorType::RGBA:
			case ColorType::RGBA16:
			case ColorType::RGBA32: {
				float colors2[4] = { 0, 0, 0, 0 };
				glClearTexImage(id, 0, GL_RGBA, GL_FLOAT, &colors2);
				break;
			}
		}
	}

	void Texture::mipmap2D(unsigned int id) {
		glBindTexture(GL_TEXTURE_2D, id);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::mipmapCube(unsigned int id) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}


	void Texture::setMaxAnisotropyAmount(AnisotropicFilter value) {
		maxAnisotropy = value;
	}


	void Texture2D::initColorType(ColorType type, int width, int height, unsigned char* image) {
		switch (type) {
			case ColorType::GammaSpace:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
				break;
			case ColorType::Single:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				break;
			case ColorType::RGB:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				break;
			case ColorType::RGBA:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
				break;
			case ColorType::RGB16:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, image);
				break;
			case ColorType::RGBA16:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, image);
				break;
			case ColorType::RGBA32:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, image);
				break;
		}
	}


	void Texture2D::initFilterMode(FilterMode mode) {
		switch (mode) {
			case FilterMode::None:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case FilterMode::Nearest:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case FilterMode::Linear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case FilterMode::Bilinear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case FilterMode::Trilinear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			}
	}


	void Texture2D::initWrapMode(WrapMode mode) {
		switch (mode) {
			case WrapMode::Repeat:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					break;
			case WrapMode::MirrorRepeat:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				break;
			case WrapMode::ClampEdge:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case WrapMode::ClampBorder:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				break;
		}
	}

	void Texture2D::initAnisotropyFilter(AnisotropicFilter filter) {
		float maxValue = 0.f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxValue);

		maxValue = std::fmin(maxValue, (float)filter);
		maxValue = std::fmin(maxValue, (float)maxAnisotropy);

		if(maxValue > 0.f)
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxValue);
	}

	void Texture2D::mipmap() const {
		glBindTexture(GL_TEXTURE_2D, getID());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::bind() const {
		glBindTexture(GL_TEXTURE_2D, getID());
	}

	

}