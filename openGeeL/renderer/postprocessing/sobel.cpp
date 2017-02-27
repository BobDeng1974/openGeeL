#include "sobel.h"

namespace geeL {

	SobelFilter::SobelFilter(float scale) 
		: PostProcessingEffect("renderer/postprocessing/sobel.frag"), scale(scale) {}


	void SobelFilter::bindValues() {
		PostProcessingEffect::bindValues();

		shader.setFloat("scale", scale);
	}
}