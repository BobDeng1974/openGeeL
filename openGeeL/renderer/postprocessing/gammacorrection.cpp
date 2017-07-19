#define GLEW_STATIC
#include <glew.h>
#include "../shader/rendershader.h"
#include "gammacorrection.h"

namespace geeL {

	GammaCorrection::GammaCorrection(float gamma) : 
		PostProcessingEffect("renderer/postprocessing/gammacorrection.frag"), gamma(gamma) {}


	void GammaCorrection::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffect::init(screen, buffer, resolution);

		shader.setFloat("gamma", gamma);
	}

}