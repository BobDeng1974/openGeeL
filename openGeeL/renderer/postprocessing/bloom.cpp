#define GLEW_STATIC
#include <glew.h>
#include "gaussianblur.h"
#include "../utility/screenquad.h"
#include "bloom.h"

namespace geeL {

	BloomFilter::BloomFilter(float scatter)
		: PostProcessingEffect("renderer/postprocessing/bloomfilter.frag"), scatter(scatter) {}


	void BloomFilter::bindValues() {
		PostProcessingEffect::bindValues();

		shader.setFloat("scatter", scatter);
	}

	Bloom::Bloom(GaussianBlur& blur, float scatter) 
		: 
		PostProcessingEffect("renderer/postprocessing/bloomcombine.frag"),
		blur(blur), filter(new BloomFilter(scatter)) {}


	Bloom::~Bloom() {
		delete filter;
	}

	void Bloom::setScatter(float scatter) {
		filter->scatter = scatter;
	}

	void Bloom::setScreen(ScreenQuad& screen) {
		PostProcessingEffect::setScreen(screen);

		filterBuffer.init(screen.width, screen.height);
		blurBuffer.init(screen.width, screen.height);

		filter->setScreen(screen);
		blur.setScreen(screen);

		blur.setParentFBO(blurBuffer.fbo);

		//Assign buffer that the blurred and cutout bloom image will be rendered to
		buffers.push_back(blurBuffer.getColorID());
	}

	void Bloom::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("bloom", shader.mapOffset + 1);

		filter->setBuffer(buffers.front());
		filterBuffer.fill(*filter);

		blur.setBuffer(filterBuffer.getColorID());
		blurBuffer.fill(blur);

		FrameBuffer::bind(parentFBO);
	}


}