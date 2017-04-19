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


	void BlurredPostEffect::setBuffer(const ColorBuffer& buffer) {
		PostProcessingEffect::setBuffer(buffer);

		effect.setBuffer(buffer);
	}

	void BlurredPostEffect::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);
		screenInfo = &info;

		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("image2", shader.mapOffset + 1);
		shader.setInteger("effectOnly", onlyEffect);

		effectBuffer.init(unsigned int(info.width * effectResolution), unsigned int(info.height * effectResolution),
			ColorType::RGB16, FilterMode::Linear);
		blurBuffer.init(unsigned int(info.width * blurResolution), unsigned int(info.height * blurResolution),
			ColorType::RGB16, FilterMode::Linear);

		effect.init(screen, effectBuffer.info);
		blur.init(screen, blurBuffer.info);

		buffers.push_back(blurBuffer.getTexture().getID());
	}


	void BlurredPostEffect::bindValues() {
		effectBuffer.fill(effect);

		blur.setBuffer(effectBuffer.getTexture().getID());
		blurBuffer.fill(blur);

		ColorBuffer::resetSize(screenInfo->width, screenInfo->height);
		ColorBuffer::bind(parentFBO);
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