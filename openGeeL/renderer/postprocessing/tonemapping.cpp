#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "tonemapping.h"

namespace geeL {

	ToneMapping::ToneMapping(float exposure) 
		: PostProcessingEffect("renderer/postprocessing/tonemapping.frag"), exposure(exposure) {}


	void ToneMapping::bindValues() {
		PostProcessingEffect::bindValues();

		shader.setFloat("exposure", exposure);
	}

}