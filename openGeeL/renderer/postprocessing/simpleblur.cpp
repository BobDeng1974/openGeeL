#include "simpleblur.h"

namespace geeL {

	SimpleBlur::SimpleBlur(unsigned int strength)
		: PostProcessingEffect("renderer/postprocessing/simpleblur.frag"), amount(strength + 1) {}

	SimpleBlur::SimpleBlur(unsigned int strength, std::string shaderPath)
		: PostProcessingEffect(shaderPath), amount(strength + 1) {}


	void SimpleBlur::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		shader.setInteger("amount", amount);
	}


	SimpleBlur3D::SimpleBlur3D(unsigned int strength) 
		: SimpleBlur(strength, "renderer/postprocessing/simpleblur3d.frag") {}

}