#define GLEW_STATIC
#include <glew.h>
#include <algorithm>
#include <iostream>
#include <limits.h>
#include "glwrapper/viewport.h"
#include "textureparams.h"
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

	void Texture::bind(unsigned int layer) const {
		glActiveTexture(GL_TEXTURE0 + layer);

		if (parameters != nullptr)
			parameters->bind(layer);
		else 
			TextureParameters::unbind(layer);

		bind();
	}

	void Texture::bindImage(unsigned int position, AccessType access) const {
		glBindImageTexture(position, getID(), 0, GL_TRUE, 0, (int)access, (int)getColorType());
	}

	void Texture::disable() const {
		glDisable((int)getTextureType());
	}

	void Texture::clear() {
		switch (colorType) {
			case ColorType::Single:
			case ColorType::Single16:
			case ColorType::RG16:
			case ColorType::RGB:
			case ColorType::RGB16:
			case ColorType::RGB32: {
				float colors[3] = { 0, 0, 0 };
				glClearTexImage(getID(), 0, GL_RGB, GL_FLOAT, &colors);
				break;
			}
			case ColorType::GammaSpace:
			case ColorType::RGBA:
			case ColorType::RGBA16:
			case ColorType::RGBA32: {
				float colors2[4] = { 0, 0, 0, 0 };
				glClearTexImage(getID(), 0, GL_RGBA, GL_FLOAT, &colors2);
				break;
			}
			case ColorType::Depth:
			case ColorType::Depth32:
				float colors[1] = { 0 };
				glClearTexImage(getID(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, &colors);
				break;
		}
	}

	void Texture2D::initStorage(unsigned char* image) {
		int textureType = (int)getTextureType();
		int width  = resolution.getWidth();
		int height = resolution.getHeight();

		unsigned int a = 0;
		unsigned int b = 0;
		unsigned int c = 0;

		switch (colorType) {
			case ColorType::GammaSpace:
				a = GL_SRGB_ALPHA; b = GL_RGBA, c = GL_UNSIGNED_BYTE;
				break;
			case ColorType::Depth:
				a = GL_DEPTH_COMPONENT; b = GL_DEPTH_COMPONENT, c = GL_FLOAT;
				break;
			case ColorType::Depth32:
				a = GL_DEPTH_COMPONENT32; b = GL_DEPTH_COMPONENT, c = GL_FLOAT;
				break;
			case ColorType::Single:
				a = GL_RED; b = GL_RGB, c = GL_UNSIGNED_BYTE;
				break;
			case ColorType::Single16:
				a = GL_R16; b = GL_RGB, c = GL_FLOAT;
				break;
			case ColorType::RG16:
				a = GL_RG16; b = GL_RG, c = GL_FLOAT;
				break;
			case ColorType::RGB:
				a = GL_RGB; b = GL_RGB, c = GL_UNSIGNED_BYTE;
				break;
			case ColorType::RGB16:
				a = GL_RGB16F; b = GL_RGB, c = GL_FLOAT;
				break;
			case ColorType::RGB32:
				a = GL_RGB32F; b = GL_RGB, c = GL_FLOAT;
				break;
			case ColorType::RGBA:
				a = GL_RGBA; b = GL_RGBA, c = GL_UNSIGNED_BYTE;
				break;
			case ColorType::RGBA16:
				a = GL_RGBA16F; b = GL_RGBA, c = GL_FLOAT;
				break;
			case ColorType::RGBA32:
				a = GL_RGBA32F; b = GL_RGBA, c = GL_FLOAT;
				break;
		}

		glTexImage2D(textureType, 0, a, width, height, 0, b, c, image);
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
			case FilterMode::TrilinearUltra:
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

	void Texture::setBorderColors(float r, float g, float b, float a) {
		bind();
		float borderColor[] = { r, g, b, a };
		glTexParameterfv((int)getTextureType(), GL_TEXTURE_BORDER_COLOR, borderColor);
		unbind();
	}


	void Texture::setMaxAnisotropyAmount(AnisotropicFilter value) {
		maxAnisotropy = value;
	}

	AnisotropicFilter Texture::getMaxAnisotropyAmount() {
		return maxAnisotropy;
	}

	void Texture::attachParameters(const TextureParameters& parameters) {
		this->parameters = &parameters;
	}

	void Texture::detachParameters() {
		this->parameters = nullptr;
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

	void TextureCube::initStorage(unsigned char* image) {
		int textureType = (int)getTextureType();
		unsigned int res = getResolution();

		unsigned int a = 0;
		unsigned int b = 0;
		unsigned int c = 0;

		switch (colorType) {
			case ColorType::GammaSpace:
				a = GL_SRGB_ALPHA; b = GL_RGBA, c = GL_UNSIGNED_BYTE;
				break;
			case ColorType::Depth:
				a = GL_DEPTH_COMPONENT; b = GL_DEPTH_COMPONENT, c = GL_FLOAT;
				break;
			case ColorType::Depth32:
				a = GL_DEPTH_COMPONENT32; b = GL_DEPTH_COMPONENT, c = GL_FLOAT;
				break;
			case ColorType::Single:
				a = GL_RED; b = GL_RGB, c = GL_UNSIGNED_BYTE;
				break;
			case ColorType::Single16:
				a = GL_R16; b = GL_RGB, c = GL_FLOAT;
				break;
			case ColorType::RG16:
				a = GL_RG16; b = GL_RG, c = GL_FLOAT;
				break;
			case ColorType::RGB:
				a = GL_RGB; b = GL_RGB, c = GL_UNSIGNED_BYTE;
				break;
			case ColorType::RGB16:
				a = GL_RGB16F; b = GL_RGB, c = GL_FLOAT;
				break;
			case ColorType::RGB32:
				a = GL_RGB32F; b = GL_RGB, c = GL_FLOAT;
				break;
			case ColorType::RGBA:
				a = GL_RGBA; b = GL_RGBA, c = GL_UNSIGNED_BYTE;
				break;
			case ColorType::RGBA16:
				a = GL_RGBA16F; b = GL_RGBA, c = GL_FLOAT;
				break;
			case ColorType::RGBA32:
				a = GL_RGBA32F; b = GL_RGBA, c = GL_FLOAT;
				break;
		}

		for (int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, a, res, res, 0, b, c, image);

	}


	

}