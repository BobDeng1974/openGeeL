#define GLEW_STATIC
#include <glew.h>
#include "shader/rendershader.h"
#include "gammacorrection.h"

namespace geeL {

	GammaCorrection::GammaCorrection(float gamma) : 
		PostProcessingEffectFS("renderer/postprocessing/gammacorrection.frag"), gamma(gamma) {}


	void GammaCorrection::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		shader.bind<float>("gamma", gamma);
	}

}