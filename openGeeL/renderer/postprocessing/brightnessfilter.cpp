#include "brightnessfilter.h"

namespace geeL {

	BrightnessFilter::BrightnessFilter(float scatter)
		: PostProcessingEffect("renderer/postprocessing/bloomfilter.frag"), scatter(scatter) {}


	void BrightnessFilter::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		scatterLocation = shader.setFloat("scatter", scatter);
	}

	void BrightnessFilter::setScatter(float scatter) {
		if (scatter > 0.f && scatter != this->scatter) {
			this->scatter = scatter;

			shader.use();
			shader.setFloat(scatterLocation, scatter);
		}
	}

	float BrightnessFilter::getScatter() const {
		return scatter;
	}

}