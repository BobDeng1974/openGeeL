#include "gaussianblur.h"
#include "lensflare.h"

namespace geeL {

	LensFlareFilter::LensFlareFilter(BrightnessFilter& filter, float resolution, float scale, float samples)
		: PostProcessingEffect("renderer/postprocessing/lensflare.frag"), 
			filter(filter), resolution(resolution), scale(scale), samples(samples) {}


	void LensFlareFilter::setBuffer(const Texture& texture) {
		filter.setBuffer(texture);
	}

	void LensFlareFilter::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);
		screenInfo = &buffer.info;

		shader.setFloat("scale", scale);
		shader.setFloat("samples", samples);

		filterBuffer.init(unsigned int(screenInfo->width * resolution), unsigned int(screenInfo->height * resolution),
			ColorType::RGB16, FilterMode::Linear, WrapMode::Repeat);

		filter.init(screen, filterBuffer);

		addBuffer(filterBuffer.getTexture(), "image");
	}

	float LensFlareFilter::getScale() const {
		return scale;
	}

	float LensFlareFilter::getMaxSamples() const {
		return samples;
	}

	void LensFlareFilter::setScale(float value) {
		if (scale != value && value > 0.f) {
			scale = value;

			shader.use();
			shader.setFloat("scale", scale);
		}
	}

	void LensFlareFilter::setMaxSamples(float value) {
		if (samples != value && value > 0.f) {
			samples = value;

			shader.use();
			shader.setFloat("samples", samples);
		}
	}

	void LensFlareFilter::setResolution(float value) {
		if (resolution != value && value > 0.f && resolution <= 1.f)
			resolution = value;
	}

	void LensFlareFilter::bindValues() {
		filterBuffer.fill(filter);

		FrameBuffer::resetSize(screenInfo->width, screenInfo->height);
		parentBuffer->bind();
		shader.use();
	}


	LensFlare::LensFlare(LensFlareFilter& filter, GaussianBlurBase& blur, float effectResolution, float blurResolution) 
		: BlurredPostEffect(filter, blur, effectResolution, blurResolution), filter(filter) {
	
		filter.setResolution(effectResolution);
	}

}