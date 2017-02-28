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

	Bloom::Bloom(BloomFilter& filter, GaussianBlur& blur, float filterResolution, float blurResolution)
		: BlurredPostEffect(filter, blur, filterResolution, blurResolution) {}

}