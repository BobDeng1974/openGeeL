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
				0) {}

	RenderTexture::RenderTexture(const Texture& other, Resolution resolution) 
		: Texture2D(resolution, other.getColorType(), 0) {

		assert(other.getTextureType() == TextureType::Texture2D && "Texture has to be a 2D texture");
		id = other.getTextureToken();
	}

	void RenderTexture::setRenderResolution() const {
		Texture2D::setRenderResolution();
	}

	const Resolution& RenderTexture::getResolution() const {
		return Texture2D::getResolution();
	}


	unsigned int RenderTexture::getSize() const {
		return 1;
	}

	void RenderTexture::resize(Resolution resolution) {
		glBindTexture(GL_TEXTURE_2D, id);

		setResolution(resolution);
		initStorage(0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void RenderTexture::assignInner(unsigned int position) const {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + position, GL_TEXTURE_2D, getID(), 0);
	}

}