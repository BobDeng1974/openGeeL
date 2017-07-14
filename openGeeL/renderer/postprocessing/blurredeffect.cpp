#include "../primitives/screenquad.h"
#include "../framebuffer/framebuffer.h"
#include "gaussianblur.h"
#include "blurredeffect.h"

using namespace std;

namespace geeL {

	BlurredPostEffect::BlurredPostEffect(PostProcessingEffect& effect, 
		PostProcessingEffect& blur, Resolution effectResolution, Resolution blurResolution)
			: PostProcessingEffect("renderer/postprocessing/combine.frag"),
				effect(effect), blur(blur), effectResolution(effectResolution), blurResolution(blurResolution){

		effect.effectOnly(true);
	}


	void BlurredPostEffect::setImageBuffer(const Texture& texture) {
		PostProcessingEffect::setImageBuffer(texture);

		effect.setImageBuffer(texture);
	}

	void BlurredPostEffect::init(ScreenQuad& screen, const ColorBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);
		screenInfo = &buffer.info;

		shader.setInteger("effectOnly", onlyEffect);

		effectBuffer.init(unsigned int(screenInfo->width * effectResolution), unsigned int(screenInfo->height * effectResolution),
			ColorType::RGB16, FilterMode::Linear, WrapMode::ClampEdge);
		blurBuffer.init(unsigned int(screenInfo->width * blurResolution), unsigned int(screenInfo->height * blurResolution),
			ColorType::RGB16, FilterMode::Linear, WrapMode::ClampEdge);

		effect.init(screen, effectBuffer);
		blur.init(screen, blurBuffer);
		blur.setImageBuffer(effectBuffer);

		addImageBuffer(blurBuffer.getTexture(), "image2");
	}


	void BlurredPostEffect::bindValues() {
		effectBuffer.fill(effect);
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

	const Resolution& BlurredPostEffect::getEffectResolution() const {
		return effectResolution;
	}

	const Resolution& BlurredPostEffect::getBlurResolution() const {
		return blurResolution;
	}
}