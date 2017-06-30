#include "../primitives/screenquad.h"
#include "../framebuffer/framebuffer.h"
#include "gaussianblur.h"
#include "blurredeffect.h"

using namespace std;

namespace geeL {

	BlurredPostEffect::BlurredPostEffect(PostProcessingEffect& effect, 
		PostProcessingEffect& blur, float effectResolution, float blurResolution)
			: PostProcessingEffect("renderer/postprocessing/combine.frag"),
				effect(effect), blur(blur), effectResolution(effectResolution), blurResolution(blurResolution){

		effect.effectOnly(true);
	}


	void BlurredPostEffect::setBuffer(unsigned int buffer) {
		PostProcessingEffect::setBuffer(buffer);

		effect.setBuffer(buffer);
	}

	void BlurredPostEffect::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);
		screenInfo = &buffer.info;

		shader.setInteger("effectOnly", onlyEffect);

		effectBuffer.init(unsigned int(screenInfo->width * effectResolution), unsigned int(screenInfo->height * effectResolution),
			ColorType::RGB16, FilterMode::Linear);
		blurBuffer.init(unsigned int(screenInfo->width * blurResolution), unsigned int(screenInfo->height * blurResolution),
			ColorType::RGB16, FilterMode::Linear);

		effect.init(screen, effectBuffer);
		blur.init(screen, blurBuffer);

		addBuffer(blurBuffer.getTexture(), "image2");
	}


	void BlurredPostEffect::bindValues() {
		effectBuffer.fill(effect);

		blur.setBuffer(effectBuffer);
		blurBuffer.fill(blur);

		FrameBuffer::resetSize(screenInfo->width, screenInfo->height);
		parentBuffer->bind();
	}

	void BlurredPostEffect::draw() {
		shader.use();
		bindValues();

		//Switch shader again since shader of encapsuled 
		//effect was set active during 'bindValues'
		shader.use();
		bindToScreen();
	}

	void BlurredPostEffect::resizeEffectResolution(float effectResolution) {
		if (effectResolution > 0.f && effectResolution < 1.f) {
			this->effectResolution = effectResolution;

			effectBuffer.resize(unsigned int(screenInfo->width * effectResolution), 
				unsigned int(screenInfo->height * effectResolution));
		}
	}

	float BlurredPostEffect::getEffectResolution() const {
		return effectResolution;
	}

	float BlurredPostEffect::getBlurResolution() const {
		return blurResolution;
	}
}