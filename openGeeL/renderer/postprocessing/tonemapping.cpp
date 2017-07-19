#define GLEW_STATIC
#include <glew.h>
#include "../shader/rendershader.h"
#include "tonemapping.h"

namespace geeL {

	ToneMapping::ToneMapping(float exposure) 
		: PostProcessingEffect("renderer/postprocessing/tonemapping.frag"), exposure(exposure) {}


	void ToneMapping::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffect::init(screen, buffer, resolution);

		shader.setFloat("exposure", exposure);
	}
}