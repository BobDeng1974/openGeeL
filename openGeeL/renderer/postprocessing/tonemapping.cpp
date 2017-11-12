#define GLEW_STATIC
#include <glew.h>
#include "shader/rendershader.h"
#include "tonemapping.h"

namespace geeL {

	ToneMapping::ToneMapping(float exposure) 
		: PostProcessingEffectFS("renderer/postprocessing/tonemapping.frag")
		, exposure(exposure) {}


	void ToneMapping::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		shader.bind<float>("exposure", exposure);
	}
}