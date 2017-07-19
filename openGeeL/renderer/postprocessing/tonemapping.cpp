#define GLEW_STATIC
#include <glew.h>
#include "../shader/rendershader.h"
#include "tonemapping.h"

namespace geeL {

	ToneMapping::ToneMapping(float exposure) 
		: PostProcessingEffectFS("renderer/postprocessing/tonemapping.frag"), exposure(exposure) {}


	void ToneMapping::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.setFloat("exposure", exposure);
	}
}