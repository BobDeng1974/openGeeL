#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "tonemapping.h"

namespace geeL {

	ToneMapping::ToneMapping(float exposure) 
		: PostProcessingEffect("renderer/postprocessing/tonemapping.frag"), exposure(exposure) {}


	void ToneMapping::draw() {

		shader.use();
		glUniform1f(glGetUniformLocation(shader.program, "exposure"), exposure);
	}

}