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

	void BlurredPostEffect::init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffect::init(screen, buffer, resolution);

		shader.setInteger("effectOnly", onlyEffect);

		Resolution effectRes = Resolution(resolution, effectResolution);
		effectTexture = new RenderTexture(effectRes,
			ColorType::RGB16, WrapMode::ClampEdge, FilterMode::Linear);

		Resolution blurRes = Resolution(resolution, blurResolution);
		blurTexture = new RenderTexture(blurRes,
			ColorType::RGB16, WrapMode::ClampEdge, FilterMode::Linear);

		effectBuffer.init(effectRes, *effectTexture);
		blurBuffer.init(blurRes, *blurTexture);

		effect.init(screen, effectBuffer, effectRes);
		blur.init(screen, blurBuffer, blurRes);

		blur.setImageBuffer(*effectTexture);
		addImageBuffer(*blurTexture, "image2");
	}


	void BlurredPostEffect::bindValues() {
		effectBuffer.fill(effect);
		blurBuffer.fill(blur);

		resolution.setRenderResolution();
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