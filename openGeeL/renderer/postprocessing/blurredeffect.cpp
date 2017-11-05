#include "primitives/screenquad.h"
#include "texturing/rendertexture.h"
#include "framebuffer/framebuffer.h"
#include "gaussianblur.h"
#include "blurredeffect.h"

using namespace std;

namespace geeL {

	BlurredPostEffect::BlurredPostEffect(PostProcessingEffectFS& effect, 
		PostProcessingEffectFS& blur, ResolutionScale effectResolution, ResolutionScale blurResolution)
			: PostProcessingEffectFS("renderer/postprocessing/combine.frag"),
				effect(effect), blur(blur), effectResolution(effectResolution), blurResolution(blurResolution){

		effect.effectOnly(true);
	}

	BlurredPostEffect::~BlurredPostEffect() {
		if (effectTexture != nullptr) delete effectTexture;
		if (blurTexture != nullptr) delete blurTexture;
	}


	void BlurredPostEffect::setImage(const Texture& texture) {
		PostProcessingEffectFS::setImage(texture);

		effect.setImage(texture);
	}

	void BlurredPostEffect::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.bind<int>("effectOnly", onlyEffect);

		Resolution effectRes = Resolution(resolution, effectResolution);
		effectTexture = new RenderTexture(effectRes, ColorType::RGB16, 
			WrapMode::ClampEdge, FilterMode::Linear);

		Resolution blurRes = Resolution(resolution, blurResolution);
		blurTexture = new RenderTexture(blurRes, ColorType::RGB16, 
			WrapMode::ClampEdge, FilterMode::Linear);

		effect.init(screen, buffer, effectRes);
		blur.init(screen, buffer, blurRes);

		blur.setImage(*effectTexture);
		addTextureSampler(*blurTexture, "image2");
	}


	void BlurredPostEffect::bindValues() {
		parentBuffer->add(*effectTexture);
		parentBuffer->fill(effect);

		parentBuffer->add(*blurTexture);
		parentBuffer->fill(blur);

	}


	void BlurredPostEffect::setRenderMask(RenderMask mask) {
		PostProcessingEffectFS::setRenderMask(mask);

		effect.setRenderMask(RenderMask::None);
		blur.setRenderMask(RenderMask::None);

		//Implementation note:
		//Masking underlying effect doesn't currently work.
		//For that, a new RBO with effect resolution ought to be created and 
		//content of parent buffers stencil buffer linearly copied into it. 
		//This RBO then would then be bound to parent buffer for every effect
		//draw, and the original RBO rebound afterwards.
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