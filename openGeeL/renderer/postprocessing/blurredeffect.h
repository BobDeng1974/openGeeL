#ifndef BLURREDEFFECT_H
#define BLURREDEFFECT_H

#include "postprocessing.h"

namespace geeL {

	class RenderTexture;

	//Wrapper effect that can blur and scale another post processing effect
	class BlurredPostEffect : public PostProcessingEffect {

	public:
		BlurredPostEffect(PostProcessingEffect& effect, PostProcessingEffect& blur, 
			ResolutionScale effectResolution = FULLSCREEN, ResolutionScale blurResolution = FULLSCREEN);
		~BlurredPostEffect();

		virtual void setImageBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution);

		virtual void draw();

		void resizeEffectResolution(ResolutionScale effectResolution);

		const ResolutionScale& getEffectResolution() const;
		const ResolutionScale& getBlurResolution() const;

	protected:
		virtual void bindValues();

	private:
		ResolutionScale effectResolution, blurResolution;
		PostProcessingEffect& effect;
		PostProcessingEffect& blur;

		RenderTexture* effectTexture = nullptr;
		RenderTexture* blurTexture = nullptr;

	};
}

#endif