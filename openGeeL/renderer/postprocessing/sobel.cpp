#include "sobel.h"

namespace geeL {

	SobelFilter::SobelFilter(float scale) 
		: PostProcessingEffect("renderer/postprocessing/sobel.frag"), scale(scale) {}


	void SobelFilter::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setFloat("scale", scale);
	}

}