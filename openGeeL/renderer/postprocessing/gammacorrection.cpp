#define GLEW_STATIC
#include <glew.h>
#include "../shader/rendershader.h"
#include "gammacorrection.h"

namespace geeL {

	GammaCorrection::GammaCorrection(float gamma) : 
		PostProcessingEffectFS("renderer/postprocessing/gammacorrection.frag"), gamma(gamma) {}


	void GammaCorrection::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.bind<float>("gamma", gamma);
	}

}