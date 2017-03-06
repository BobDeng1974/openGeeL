#define GLEW_STATIC
#include <glew.h>
#include "gaussianblur.h"
#include "../primitives/screenquad.h"
#include "../utility/framebuffer.h"
#include "bloom.h"

namespace geeL {

	BloomFilter::BloomFilter(float scatter)
		: PostProcessingEffect("renderer/postprocessing/bloomfilter.frag"), scatter(scatter) {}


	void BloomFilter::bindValues() {
		PostProcessingEffect::bindValues();

		shader.setFloat("scatter", scatter);
	}

	void BloomFilter::setScatter(float scatter) {
		if (scatter > 0.f)
			this->scatter = scatter;
	}

	float BloomFilter::getScatter() const {
		return scatter;
	}


	Bloom::Bloom(BloomFilter& filter, GaussianBlur& blur, float filterResolution, float blurResolution)
		: BlurredPostEffect(filter, blur, filterResolution, blurResolution), filter(filter) {}

	void Bloom::setScatter(float scatter) {
		filter.setScatter(scatter);
	}

	float  Bloom::getScatter() const {
		return filter.getScatter();
	}


}