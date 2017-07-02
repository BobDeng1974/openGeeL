#include "sobel.h"

namespace geeL {

	SobelFilter::SobelFilter(float scale) 
		: PostProcessingEffect("renderer/postprocessing/sobel.frag"), scale(scale) {}


	void SobelFilter::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setFloat("scale", scale);
	}

	float SobelFilter::getScale() const {
		return scale;
	}

	void SobelFilter::setScale(float value) {
		if (scale != value && value > 0.f) {
			scale = value;

			shader.use();
			shader.setFloat("scale", scale);
		}
	}

}