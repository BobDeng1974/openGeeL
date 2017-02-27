#include "../utility/screenquad.h"
#include "../utility/framebuffer.h"
#include "gaussianblur.h"
#include "blurredeffect.h"

using namespace std;

namespace geeL {

	BlurredPostEffect::BlurredPostEffect(PostProcessingEffect& effect, PostProcessingEffect& blur, float resolution)
		: PostProcessingEffect("renderer/postprocessing/combine.frag"),
		effect(effect), blur(blur), resolution(resolution) {

		effect.effectOnly(true);
	}

	BlurredPostEffect::~BlurredPostEffect() {
		if (effectScreen != nullptr)
			delete effectScreen;
	}



	void BlurredPostEffect::setBuffer(const FrameBuffer& buffer) {
		PostProcessingEffect::setBuffer(buffer);

		effect.setBuffer(buffer);
	}

	void BlurredPostEffect::init(ScreenQuad& screen) {
		PostProcessingEffect::init(screen);

		effectScreen = new ScreenQuad(screen.width * resolution, screen.height * resolution);
		effectScreen->init();

		effectBuffer.init(effectScreen->width, effectScreen->height);
		blurBuffer.init(effectScreen->width, effectScreen->height);

		effect.init(*effectScreen);
		blur.init(*effectScreen);
		blur.setParentFBO(blurBuffer.fbo);

		buffers.push_back(blurBuffer.getColorID());
	}


	void BlurredPostEffect::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("image2", shader.mapOffset + 1);
		shader.setInteger("effectOnly", onlyEffect);

		effectBuffer.fill(effect);

		blur.setBuffer(effectBuffer.getColorID());
		blurBuffer.fill(blur);

		FrameBuffer::resetSize(screen->width, screen->height);
		FrameBuffer::bind(parentFBO);
	}
}