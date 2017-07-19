#include "../primitives/screenquad.h"
#include "../texturing/rendertexture.h"
#include "../framebuffer/framebuffer.h"
#include "gaussianblur.h"
#include "blurredeffect.h"
#include <iostream>

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

	void BlurredPostEffect::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffect::init(screen, buffer, resolution);

		shader.setInteger("effectOnly", onlyEffect);

		Resolution effectRes = Resolution(resolution, effectResolution);
		effectTexture = new RenderTexture(effectRes, ColorType::RGB16, 
			WrapMode::ClampEdge, FilterMode::Linear);

		Resolution blurRes = Resolution(resolution, blurResolution);
		blurTexture = new RenderTexture(blurRes, ColorType::RGB16, 
			WrapMode::ClampEdge, FilterMode::Linear);

		effect.init(screen, buffer, effectRes);
		blur.init(screen, buffer, blurRes);

		blur.setImageBuffer(*effectTexture);
		addImageBuffer(*blurTexture, "image2");
	}


	void BlurredPostEffect::bindValues() {
		parentBuffer->add(*effectTexture);
		parentBuffer->fill(effect);

		parentBuffer->add(*blurTexture);
		parentBuffer->fill(blur);

	}

	void BlurredPostEffect::draw() {
		bindValues();

		shader.use();
		bindToScreen();
	}

	void BlurredPostEffect::resizeEffectResolution(ResolutionScale effectResolution) {
		this->effectResolution = effectResolution;

		Resolution newRes = Resolution(resolution, effectResolution);
		effect.setResolution(newRes);
		effectTexture->resize(newRes);
	}

	const ResolutionScale& BlurredPostEffect::getEffectResolution() const {
		return effectResolution;
	}

	const ResolutionScale& BlurredPostEffect::getBlurResolution() const {
		return blurResolution;
	}
}