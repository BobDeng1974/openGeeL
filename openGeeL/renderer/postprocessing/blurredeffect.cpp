#include "../utility/screenquad.h"
#include "../utility/framebuffer.h"
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


	void BlurredPostEffect::setBuffer(const FrameBuffer& buffer) {
		PostProcessingEffect::setBuffer(buffer);

		effect.setBuffer(buffer);
	}

	void BlurredPostEffect::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		screenInfo = &info;

		effectBuffer.init(info.width * effectResolution, info.height * effectResolution);
		blurBuffer.init(info.width * blurResolution, info.height * blurResolution);

		effect.init(screen, effectBuffer.info);
		blur.init(screen, blurBuffer.info);

		buffers.push_back(blurBuffer.getColorID());
	}


	void BlurredPostEffect::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("image2", shader.mapOffset + 1);
		shader.setInteger("effectOnly", onlyEffect);

		effectBuffer.fill(effect);

		blur.setBuffer(effectBuffer.getColorID());
		blurBuffer.fill(blur);

		FrameBuffer::resetSize(screenInfo->width, screenInfo->height);
		FrameBuffer::bind(parentFBO);
	}
}