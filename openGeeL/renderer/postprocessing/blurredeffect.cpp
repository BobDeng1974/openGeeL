#include "primitives/screenquad.h"
#include "texturing/rendertexture.h"
#include "framebuffer/framebuffer.h"
#include "gaussianblur.h"
#include "blurredeffect.h"

using namespace std;

namespace geeL {

	BlurredPostEffect::BlurredPostEffect(PostProcessingEffectFS& effect, 
		PostProcessingEffectFS& blur, RenderResolution effectResolution, RenderResolution blurResolution)
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

	void BlurredPostEffect::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		shader.bind<int>("effectOnly", onlyEffect);

		Resolution effectRes(resolution, effectResolution);
		if (effectTexture == nullptr)
			effectTexture = new RenderTexture(effectRes, ColorType::RGB16,
				WrapMode::ClampEdge, FilterMode::Linear);
		else
			effectTexture->resize(effectRes);

		Resolution blurRes(resolution, blurResolution);
		if (blurTexture == nullptr)
			blurTexture = new RenderTexture(blurRes, ColorType::RGB16,
				WrapMode::ClampEdge, FilterMode::Linear);
		else
			blurTexture->resize(blurRes);

		effect.init(PostProcessingParameter(parameter, effectRes));
		blur.init(PostProcessingParameter(parameter, blurRes));

		blur.setImage(*effectTexture);
		addTextureSampler(*blurTexture, "image2");
	}

	void BlurredPostEffect::bindValues() {
		effect.bindValues();
		blur.bindValues();
	}


	void BlurredPostEffect::drawSubImages() {
		if (effect.isActive()) {
			parentBuffer->add(*effectTexture);
			parentBuffer->fill(effect, clearColor);

			parentBuffer->add(*blurTexture);
			parentBuffer->fill(blur, clearColor);

		}
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


	void BlurredPostEffect::resizeEffectResolution(RenderResolution effectResolution) {
		this->effectResolution = effectResolution;

		Resolution newRes = Resolution(resolution, effectResolution);
		effect.setResolution(newRes);
		effectTexture->resize(newRes);
	}

	const RenderResolution& BlurredPostEffect::getEffectResolution() const {
		return effectResolution;
	}

	const RenderResolution& BlurredPostEffect::getBlurResolution() const {
		return blurResolution;
	}
}