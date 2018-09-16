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


	Texture::Texture(ColorType colorType, 
		FilterMode filterMode, 
		WrapMode wrapMode, 
		AnisotropicFilter filter)
			: colorType(colorType)
			, filterMode(filterMode)
			, wrapMode(wrapMode)
			, filter(filter)
			, parameters(nullptr) {}

	Texture::Texture(Texture&& other) 
		: id(std::move(other.id))
		, colorType(other.colorType)
		, parameters(other.parameters) {

		other.parameters = nullptr;
	}

	Texture& Texture::operator=(Texture&& other) {
		if (this != &other) {
			id = std::move(other.id);
			colorType = other.colorType;
			parameters = other.parameters;

			other.parameters = nullptr;
		}

		return *this;
	}


	

	void Texture::bind() const {
		glBindTexture((int)getTextureType(), id);
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
		glBindImageTexture(position, id, 0, GL_TRUE, 0, (int)access, (int)getColorType());
	}

	void Texture::disable() const {
		glDisable((int)getTextureType());
	}

	bool Texture::isEmpty() const {
		return id.operator unsigned int() == 0;
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
				glClearTexImage(id, 0, GL_RGB, GL_FLOAT, &colors);
				break;
			}
			case ColorType::GammaSpace:
			case ColorType::RGBA:
			case ColorType::RGBA8:
			case ColorType::RGBA16:
			case ColorType::RGBA32: {
				float colors2[4] = { 0, 0, 0, 0 };
				glClearTexImage(id, 0, GL_RGBA, GL_FLOAT, &colors2);
				break;
			}
			case ColorType::Depth:
			case ColorType::Depth32:
				float colors[1] = { 0 };
				glClearTexImage(id, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &colors);
				break;
		}
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

	FunctionGuard<Texture> Texture::operator->() {
		std::function<void()> enter = [this]() { this->bind(); };
		std::function<void()> exit  = [this]() { this->unbind(); };

		return FunctionGuard<Texture>(*this, enter, exit);
	}



	Texture2D::Texture2D(ColorType colorType)
		: Texture(colorType) {}

	Texture2D::Texture2D(Resolution resolution, 
		ColorType colorType, 
		FilterMode filterMode, 
		WrapMode wrapMode, 
		AnisotropicFilter filter, 
		void* image)
			: Texture(colorType, 
				filterMode, 
				wrapMode, 
				filter)
			, resolution(resolution) {
	
		bind();
		initStorage(image);
		initWrapMode(wrapMode);
		initFilterMode(filterMode);
		initAnisotropyFilter(filter);
		unbind();
	}

	Texture2D::Texture2D(Texture2D&& other)
		: Texture(std::move(other))
		, resolution(other.resolution) {}

	Texture2D& Texture2D::operator=(Texture2D&& other) {
		if (this != &other) {
			Texture::operator=(std::move(other));
			resolution = other.resolution;
		}

		return *this;
	}


	void getColorValues(ColorType type, unsigned int& a, unsigned int& b, unsigned int& c) {
		switch (type) {
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
				a = GL_R16F; b = GL_RGB, c = GL_FLOAT;
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
			case ColorType::RGBA8:
				a = GL_RGBA8; b = GL_RGBA, c = GL_FLOAT;
				break;
			case ColorType::RGBA16:
				a = GL_RGBA16F; b = GL_RGBA, c = GL_FLOAT;
				break;
			case ColorType::RGBA32:
				a = GL_RGBA32F; b = GL_RGBA, c = GL_FLOAT;
				break;
			}

	}


	void Texture2D::initStorage(void* image) {
		int width  = resolution.getWidth();
		int height = resolution.getHeight();

		unsigned int a = 0;
		unsigned int b = 0;
		unsigned int c = 0;
		getColorValues(colorType, a, b, c);

		glTexImage2D(GL_TEXTURE_2D, 0, a, width, height, 0, b, c, image);
	}

	void Texture2D::reserveStorage(unsigned int levels) {
		int width = resolution.getWidth();
		int height = resolution.getHeight();
		unsigned int level = (levels == 0) ? 1 + floor(log2(max(width, height))) : levels;

		glTexStorage2D(GL_TEXTURE_2D, level, (int)getColorType(), width, height);
	}

	void Texture2D::mipmap() const {
		glBindTexture(GL_TEXTURE_2D, id);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::resize(Resolution resolution) {
		glBindTexture(GL_TEXTURE_2D, id);
		setResolution(resolution);
		initStorage(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture2D::assign(AttachmentPosition position, MipLevel level) const {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + position, GL_TEXTURE_2D, id, level);
	}

	void Texture2D::assignDepth(MipLevel level) const {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, level);
	}

	void Texture2D::applyRenderResolution() const {
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




	Texture3D::Texture3D(unsigned int width, 
		unsigned int height, 
		unsigned int depth, 
		unsigned int levels, 
		ColorType colorType,
		FilterMode filterMode, 
		WrapMode wrapMode, 
		float* buffer)
			: Texture(colorType, filterMode, wrapMode)
			, width(width)
			, height(height)
			, depth(depth)
			, levels(levels) {

		bind();
		initFilterMode(filterMode);
		initWrapMode(wrapMode);
		initStorage(buffer);
		unbind();
	}


	Texture3D::Texture3D(Texture3D&& other)
		: Texture(std::move(other))
		, width(other.width)
		, height(other.height)
		, depth(other.depth)
		, levels(other.levels) {}

	Texture3D& Texture3D::operator=(Texture3D&& other) {
		if (this != &other) {
			Texture::operator=(std::move(other));

			width = other.width;
			height = other.height;
			depth = other.depth;
			levels = other.levels;
		}

		return *this;
	}

	void Texture3D::mipmap() const {
		glBindTexture(GL_TEXTURE_3D, id);
		glGenerateMipmap(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, 0);
	}

	void Texture3D::resize(Resolution resolution) {
		std::cout << "Resizing of 3D texture is currently not supported\n";
	}

	void Texture3D::assign(AttachmentPosition position, MipLevel level) const {
		std::cout << "Assigning of 3D texture is currently not supported\n";
	}

	void Texture3D::assignDepth(MipLevel level) const {
		std::cout << "Assigning of 3D texture is currently not supported\n";
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


	void Texture3D::initStorage(float* buffer) {

		if(buffer == 0)
			glTexStorage3D(GL_TEXTURE_3D, levels, (int)getColorType(), width, height, depth);
		else {
			//assert(buffer.size() == (4 * width * height * depth));

			unsigned int a = 0;
			unsigned int b = 0;
			unsigned int c = 0;
			getColorValues(colorType, a, b, c);

			glTexStorage3D(GL_TEXTURE_3D, levels, a, width, height, depth);
			glTexImage3D(GL_TEXTURE_3D, 0, a, width, height, depth, 0, b, c, buffer);
		}
		
	}

	void Texture3D::applyRenderResolution() const {
		Viewport::set(0, 0, width, height);
	}




	TextureCube::TextureCube(unsigned int resolution,
		ColorType colorType,
		FilterMode filterMode,
		WrapMode wrapMode,
		void* images[6])
			: Texture(colorType, filterMode, wrapMode)
			, resolution(resolution) {

		bind();
		initStorage(images);

		initFilterMode(filterMode);
		initWrapMode(wrapMode);
		unbind();
	}


	TextureCube::TextureCube(TextureCube&& other)
		: Texture(std::move(other))
		, resolution(other.resolution) {}


	TextureCube& TextureCube::operator=(TextureCube&& other) {
		if (this != &other) {
			Texture::operator=(std::move(other));
			resolution = other.resolution;
		}

		return *this;
	}

	void TextureCube::mipmap() const {
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void TextureCube::resize(Resolution resolution) {
		this->resolution = (resolution.getWidth() + resolution.getHeight()) / 2;

		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		initStorage(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void TextureCube::assign(AttachmentPosition position, MipLevel level) const {
		for (unsigned int side = 0; side < 6; side++)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + position, 
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, id, level);
	}

	void TextureCube::assignSide(AttachmentPosition position, MipLevel level, unsigned int side) const {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + position,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, id, level);
	}

	void TextureCube::assignDepth(MipLevel level) const {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, id, level);
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

	void TextureCube::initStorage(void* images[6]) {
		int textureType = (int)getTextureType();
		unsigned int res = getResolution();

		unsigned int a = 0;
		unsigned int b = 0;
		unsigned int c = 0;
		getColorValues(colorType, a, b, c);

		for (int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, a, res, res, 0, b, c, (images == 0) ? 0 : images[i]);
	}

	void TextureCube::applyRenderResolution() const {
		Viewport::set(0, 0, resolution, resolution);
	}

}