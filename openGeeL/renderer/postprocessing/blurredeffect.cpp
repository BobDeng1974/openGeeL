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

	void BlurredPostEffect::init(ScreenQuad& screen, IFrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setInteger("effectOnly", onlyEffect);

		effectBuffer.init(unsigned int(parentBuffer->getWidth() * effectResolution), unsigned int(parentBuffer->getHeight() * effectResolution),
			ColorType::RGB16, FilterMode::Linear, WrapMode::ClampEdge);
		blurBuffer.init(unsigned int(parentBuffer->getWidth() * blurResolution), unsigned int(parentBuffer->getHeight() * blurResolution),
			ColorType::RGB16, FilterMode::Linear, WrapMode::ClampEdge);

		effect.init(screen, effectBuffer);
		blur.init(screen, blurBuffer);
		blur.setImageBuffer(effectBuffer);

		addImageBuffer(blurBuffer.getTexture(), "image2");
	}


	void BlurredPostEffect::bindValues() {
		effectBuffer.fill(effect);
		blurBuffer.fill(blur);

		parentBuffer->resetSize();
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

			effectBuffer.resize(effectResolution);
		}
	}

	const Resolution& BlurredPostEffect::getEffectResolution() const {
		return effectResolution;
	}

	const Resolution& BlurredPostEffect::getBlurResolution() const {
		return blurResolution;
	}
}