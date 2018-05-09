#define GLEW_STATIC
#include <glew.h>
#include <cassert>
#include "framebuffer/framebuffer.h"
#include "rendertexture.h"
#include <iostream>

namespace geeL {
	
	RenderTexture::RenderTexture() {}
	
	RenderTexture::RenderTexture(Resolution resolution,
		ColorType colorType,
		WrapMode wrapMode,
		FilterMode filterMode)
			: FunctionalTexture(std::unique_ptr<Texture>(new Texture2D(
				resolution,
				colorType,
				filterMode,
				wrapMode,
				AnisotropicFilter::None, 0))) {}


	void RenderTexture::applyRenderResolution() const {
		getTexture().applyRenderResolution();
	}

	Resolution RenderTexture::getResolution() const {
		return getTexture().getScreenResolution();
	}

	Resolution RenderTexture::getRenderResolution() const {
		return getTexture().getScreenResolution();
	}

	unsigned int RenderTexture::getSize() const {
		return 1;
	}

	void RenderTexture::resize(Resolution resolution) {
		getTexture().resize(resolution);
	}

	ColorType RenderTexture::getColorType() const {
		return getTexture().getColorType();
	}

	ResolutionScale RenderTexture::getScale() const {
		return getTexture().getScreenResolution().getScale();
	}

	void RenderTexture::attachParameters(const TextureParameters & parameters) {
		getTexture().attachParameters(parameters);
	}

	void RenderTexture::detachParameters() {
		getTexture().detachParameters();
	}

	void RenderTexture::mipmap() const {
		getTexture().mipmap();
	}

	void RenderTexture::assign(unsigned int position) const {
		getTexture().assign(position);
	}

}