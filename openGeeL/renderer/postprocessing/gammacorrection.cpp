#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "gammacorrection.h"

namespace geeL {

	GammaCorrection::GammaCorrection(float gamma) : 
		PostProcessingEffect("renderer/postprocessing/gammacorrection.frag"), gamma(gamma) {}


	void GammaCorrection::bindValues() {
		PostProcessingEffect::bindValues();

		shader.setFloat("gamma", gamma);
	}


}