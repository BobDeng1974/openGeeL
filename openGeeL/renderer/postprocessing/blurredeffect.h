#ifndef BLURREDEFFECT_H
#define BLURREDEFFECT_H

#include "postprocessing.h"

namespace geeL {

	class RenderTexture;

	//Wrapper effect that can blur and scale another post processing effect
	class BlurredPostEffect : public PostProcessingEffectFS {

	public:
		BlurredPostEffect(PostProcessingEffectFS& effect, PostProcessingEffectFS& blur, 
			ResolutionScale effectResolution = FULLSCREEN, ResolutionScale blurResolution = FULLSCREEN);
		virtual ~BlurredPostEffect();

		virtual void setImage(const Texture& texture);
		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution);

		virtual void draw();

		void resizeEffectResolution(ResolutionScale effectResolution);

		const ResolutionScale& getEffectResolution() const;
		const ResolutionScale& getBlurResolution() const;

		virtual std::string toString() const;

	protected:
		virtual void bindValues();

	private:
		ResolutionScale effectResolution, blurResolution;
		PostProcessingEffectFS& effect;
		PostProcessingEffectFS& blur;

		RenderTexture* effectTexture = nullptr;
		RenderTexture* blurTexture = nullptr;

	};


	inline std::string BlurredPostEffect::toString() const {
		return "Blurred " + effect.toString();
	}

}

#endif