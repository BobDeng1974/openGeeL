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

	Bloom::Bloom(GaussianBlur& blur, float scatter, float blurResolution)
		: PostProcessingEffect("renderer/postprocessing/combine.frag"), 
			blurResolution(blurResolution), blur(blur), filter(new BloomFilter(scatter)), 
			blurScreen(nullptr) {}


	Bloom::~Bloom() {
		delete filter;

		if (blurScreen != nullptr)
			delete blurScreen;
	}

	void Bloom::setScatter(float scatter) {
		filter->scatter = scatter;
	}

	void Bloom::init(ScreenQuad& screen) {
		PostProcessingEffect::init(screen);

		blurScreen = new ScreenQuad(screen.width * blurResolution, screen.height * blurResolution);
		blurScreen->init();

		filterBuffer.init(screen.width, screen.height);
		blurBuffer.init(blurScreen->width, blurScreen->height);

		filter->init(screen);
		blur.init(*blurScreen);
		blur.setParentFBO(blurBuffer.fbo);

		//Assign buffer that the blurred and cutout bloom image will be rendered to
		buffers.push_back(blurBuffer.getColorID());
	}

	void Bloom::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("image2", shader.mapOffset + 1);

		filter->setBuffer(buffers.front());
		filterBuffer.fill(*filter);

		blur.setBuffer(filterBuffer.getColorID());
		blurBuffer.fill(blur);

		FrameBuffer::resetSize(screen->width, screen->height);
		FrameBuffer::bind(parentFBO);
	}
}