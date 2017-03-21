#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "gammacorrection.h"

namespace geeL {

	GammaCorrection::GammaCorrection(float gamma) : 
		PostProcessingEffect("renderer/postprocessing/gammacorrection.frag"), gamma(gamma) {}


	void GammaCorrection::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		shader.setFloat("gamma", gamma);
	}

}