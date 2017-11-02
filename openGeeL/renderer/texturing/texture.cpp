#define GLEW_STATIC
#include <glew.h>
#include <algorithm>
#include <iostream>
#include <limits.h>
#include "utility/viewport.h"
#include "texture.h"

using namespace std;

namespace geeL {

	AnisotropicFilter Texture::maxAnisotropy;


	void Texture::bind() const {
		glBindTexture((int)getTextureType(), getID());
	}

	void Texture::unbind() const {
		glBindTexture((int)getTextureType(), 0);
	}

	void Texture::bindImage(unsigned int position, AccessType access) const {
		glBindImageTexture(position, getID(), 0, GL_TRUE, 0, (int)access, (int)getColorType());
	}

	void Texture::disable() const {
		glDisable((int)getTextureType());
	}

	void Texture::clear() {
		clear(colorType, getID());
	}

	void Texture2D::initStorage(unsigned char* image) {
		int textureType = (int)getTextureType();
		int width  = resolution.getWidth();
		int height = resolution.getHeight();

		switch (colorType) {
			case ColorType::GammaSpace:
				glTexImage2D(textureType, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
				break;
			case ColorType::Single:
				glTexImage2D(textureType, 0, GL_RED, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				break;
			case ColorType::Single16:
				glTexImage2D(textureType, 0, GL_R16, width, height, 0, GL_RGB, GL_FLOAT, image);
				break;
			case ColorType::RGB:
				glTexImage2D(textureType, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				break;
			case ColorType::RGB16:
				glTexImage2D(textureType, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, image);
				break;
			case ColorType::RGB32:
				glTexImage2D(textureType, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, image);
				break;
			case ColorType::RGBA:
				glTexImage2D(textureType, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
				break;
			case ColorType::RGBA16:
				glTexImage2D(textureType, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, image);
				break;
			case ColorType::RGBA32:
				glTexImage2D(textureType, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, image);
				break;
		}
	}

	void Texture2D::reserveStorage(unsigned int levels) {
		int width = resolution.getWidth();
		int height = resolution.getHeight();
		unsigned int level = (levels == 0) ? 1 + floor(log2(max(width, height))) : levels;

		glTexStorage2D(GL_TEXTURE_2D, level, (int)getColorType(), width, height);
	}

	void Texture::initFilterMode(FilterMode mode) {
		int textureType = (int)getTextureType();

		switch (mode) {
			case FilterMode::None:
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case FilterMode::Nearest:
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case FilterMode::Linear:
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case FilterMode::LinearMip:
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case FilterMode::Bilinear:
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case FilterMode::Trilinear:
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
		}
	}

	void Texture::initWrapMode(WrapMode mode) {
		std::cout << "This texture type doens't support wrapping\n";
	}

	void Texture::initAnisotropyFilter(AnisotropicFilter filter) {
		float maxValue = 0.f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxValue);

		maxValue = std::fmin(maxValue, (float)filter);
		maxValue = std::fmin(maxValue, (float)maxAnisotropy);

		if (maxValue > 0.f)
			glTexParameterf((int)getTextureType(), GL_TEXTURE_MAX_ANISOTROPY_EXT, maxValue);
	}


	void Texture::clear(ColorType type, unsigned int id) {
		switch (type) {
			case ColorType::Single:
			case ColorType::Single16:
			case ColorType::RGB:
			case ColorType::RGB16:
			case ColorType::RGB32: {
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

	void Texture::mipmap(TextureType type, unsigned int id) {
		int t = (int)type;

		glBindTexture(t, id);
		glGenerateMipmap(t);
		glBindTexture(t, 0);
	}

	void Texture::setMaxAnisotropyAmount(AnisotropicFilter value) {
		maxAnisotropy = value;
	}


	void Texture2D::mipmap() const {
		glBindTexture(GL_TEXTURE_2D, getID());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::setRenderResolution() const {
		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());
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

	void Texture2D::unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	void Texture3D::mipmap() const {
		glBindTexture(GL_TEXTURE_3D, getID());
		glGenerateMipmap(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, 0);
	}

	void Texture3D::initWrapMode(WrapMode mode) {
		switch (mode) {
			case WrapMode::Repeat:
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
				break;
			case WrapMode::MirrorRepeat:
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
				break;
			case WrapMode::ClampEdge:
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				break;
			case WrapMode::ClampBorder:
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
				break;
		}
	}

	void Texture3D::unbind() {
		glBindTexture(GL_TEXTURE_3D, 0);
	}


	void TextureCube::mipmap() const {
		glBindTexture(GL_TEXTURE_CUBE_MAP, getID());
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void TextureCube::initWrapMode(WrapMode mode) {
		switch (mode) {
			case WrapMode::Repeat:
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
				break;
			case WrapMode::MirrorRepeat:
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
				break;
			case WrapMode::ClampEdge:
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				break;
			case WrapMode::ClampBorder:
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
				break;
		}
	}

	void TextureCube::unbind() {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}


	

}