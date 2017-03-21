#include "sobel.h"

namespace geeL {

	SobelFilter::SobelFilter(float scale) 
		: PostProcessingEffect("renderer/postprocessing/sobel.frag"), scale(scale) {}


	void SobelFilter::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		shader.setFloat("scale", scale);
	}

}