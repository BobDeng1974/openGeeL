#define GLEW_STATIC
#include <glew.h>
#include "rendertexture.h"

namespace geeL {

	RenderTexture::RenderTexture(Resolution resolution,
		ColorType colorType, WrapMode wrapMode, FilterMode filterMode) 
			: Texture2D(colorType, resolution) {

		glBindTexture(GL_TEXTURE_2D, id);
		initColorType(resolution.getWidth(), resolution.getHeight(), 0);
		initWrapMode(wrapMode);
		initFilterMode(filterMode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	void RenderTexture::resize(Resolution resolution) {
		glBindTexture(GL_TEXTURE_2D, id);

		initColorType(resolution.getWidth(), resolution.getHeight(), 0);
		setResolution(resolution);
		
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}