#define GLEW_STATIC
#include <glew.h>
#include "texture.h"
#include "textureparams.h"

namespace geeL {

	TextureParameters::TextureParameters(FilterMode filterMode, WrapMode wrapMode, AnisotropicFilter aFilter)
		: id(0), filterMode(filterMode), wrapMode(wrapMode), aFilter(aFilter) {

		glGenSamplers(1, &id);

		initFilterMode(filterMode);
		initWrapMode(wrapMode);
		initAnisotropyFilter(aFilter);
	}

	TextureParameters::TextureParameters(TextureParameters&& other) 
		: id(other.id), filterMode(other.filterMode), wrapMode(other.wrapMode), aFilter(other.aFilter) {

		other.id = 0;
	}

	TextureParameters::~TextureParameters() {
		remove();
	}

	TextureParameters& TextureParameters::operator=(TextureParameters&& other) {
		if (this != &other) {
			remove();
			
			id = other.id;
			other.id = 0;

			filterMode = other.filterMode;
			wrapMode = other.wrapMode;
			aFilter = other.aFilter;
		}

		*this;
	}

	void TextureParameters::bind(unsigned int layer) const {
		glBindSampler(layer, id);
	}

	void TextureParameters::initFilterMode(FilterMode mode) {
		switch (mode) {
			case FilterMode::None:
				glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case FilterMode::Nearest:
				glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case FilterMode::Linear:
				glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case FilterMode::LinearMip:
				glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case FilterMode::Bilinear:
				glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
				glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case FilterMode::Trilinear:
				glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
		}
	}

	void TextureParameters::initWrapMode(WrapMode mode) {
		switch (mode) {
			case WrapMode::Repeat:
				glSamplerParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glSamplerParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glSamplerParameteri(id, GL_TEXTURE_WRAP_R, GL_REPEAT);
				break;
			case WrapMode::MirrorRepeat:
				glSamplerParameteri(id, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glSamplerParameteri(id, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				glSamplerParameteri(id, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
				break;
			case WrapMode::ClampEdge:
				glSamplerParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glSamplerParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glSamplerParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				break;
			case WrapMode::ClampBorder:
				glSamplerParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glSamplerParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				glSamplerParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
				break;
		}
	}

	void TextureParameters::initAnisotropyFilter(AnisotropicFilter filter) {
		float maxValue = 0.f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxValue);

		maxValue = std::fmin(maxValue, (float)filter);
		maxValue = std::fmin(maxValue, (float)Texture::getMaxAnisotropyAmount());

		if (maxValue > 0.f)
			glSamplerParameterf(id, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxValue);
	}

	void TextureParameters::remove() {
		if (id != 0)
			glDeleteSamplers(1, &id);
	}

}