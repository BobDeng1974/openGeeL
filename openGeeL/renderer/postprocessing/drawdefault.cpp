#include "../shader/shader.h"
#include "drawdefault.h"

namespace geeL {

	DefaultPostProcess::DefaultPostProcess(float exposure)
		: PostProcessingEffect("renderer/postprocessing/drawdefault.frag"), exposure(exposure) {}


	void DefaultPostProcess::bindValues() {
		PostProcessingEffect::bindValues();

		shader.setFloat("exposure", exposure);
	}

}