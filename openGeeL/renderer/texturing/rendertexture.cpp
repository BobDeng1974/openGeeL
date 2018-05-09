#define GLEW_STATIC
#include <glew.h>
#include <cassert>
#include "framebuffer/framebuffer.h"
#include "rendertexture.h"

namespace geeL {
	
	RenderTexture::RenderTexture()
		: Texture2D(ColorType::None) {}
	
	RenderTexture::RenderTexture(Resolution resolution,
		ColorType colorType, 
		WrapMode wrapMode, 
		FilterMode filterMode) 
			: Texture2D(resolution, 
				colorType, 
				filterMode, 
				wrapMode,
				AnisotropicFilter::None,
				0) {}

	void RenderTexture::applyRenderResolution() const {
		Texture2D::applyRenderResolution();
	}

	Resolution RenderTexture::getRenderPreset() const {
		return Texture2D::getResolution();
	}


	unsigned int RenderTexture::getSize() const {
		return 1;
	}

	void RenderTexture::assign(unsigned int position) const {
		Texture2D::assign(position);
	}

}