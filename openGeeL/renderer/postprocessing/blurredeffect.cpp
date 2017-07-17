#include "../primitives/screenquad.h"
#include "../texturing/rendertexture.h"
#include "../framebuffer/framebuffer.h"
#include "gaussianblur.h"
#include "blurredeffect.h"

using namespace std;

namespace geeL {

	BlurredPostEffect::BlurredPostEffect(PostProcessingEffect& effect, 
		PostProcessingEffect& blur, ResolutionScale effectResolution, ResolutionScale blurResolution)
			: PostProcessingEffect("renderer/postprocessing/combine.frag"),
				effect(effect), blur(blur), effectResolution(effectResolution), blurResolution(blurResolution){

		effect.effectOnly(true);
	}

	BlurredPostEffect::~BlurredPostEffect() {
		if (effectTexture != nullptr) delete effectTexture;
		if (blurTexture != nullptr) delete blurTexture;
	}


	void BlurredPostEffect::setImageBuffer(const Texture& texture) {
		PostProcessingEffect::setImageBuffer(texture);

		effect.setImageBuffer(texture);
	}

	void BlurredPostEffect::init(ScreenQuad& screen, IFrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setInteger("effectOnly", onlyEffect);

		effectTexture = new RenderTexture(Resolution(parentBuffer->getResolution(), effectResolution),
			ColorType::RGB16, WrapMode::ClampEdge, FilterMode::Linear);

		blurTexture = new RenderTexture(Resolution(parentBuffer->getResolution(), blurResolution),
			ColorType::RGB16, WrapMode::ClampEdge, FilterMode::Linear);

		effectBuffer.init(Resolution(parentBuffer->getResolution(), effectResolution), *effectTexture);
		blurBuffer.init(Resolution(parentBuffer->getResolution(), blurResolution), *blurTexture);

		effect.init(screen, effectBuffer);
		blur.init(screen, blurBuffer);

		blur.setImageBuffer(*effectTexture);
		addImageBuffer(*blurTexture, "image2");
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

	void BlurredPostEffect::resizeEffectResolution(ResolutionScale effectResolution) {
		this->effectResolution = effectResolution;
		effectBuffer.resize(effectResolution);
	}

	const ResolutionScale& BlurredPostEffect::getEffectResolution() const {
		return effectResolution;
	}

	const ResolutionScale& BlurredPostEffect::getBlurResolution() const {
		return blurResolution;
	}
}