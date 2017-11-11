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
		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();

		//Add a render mask to this effect. The effect will then only 
		//be drawn in regions that have been marked with given mask
		//Note: The underlying effect and blur won't be affected by the
		//mask and setting their masks manually won't work either.
		virtual void setRenderMask(RenderMask mask);

		void resizeEffectResolution(ResolutionScale effectResolution);
		void resizeEffectResolution(RenderResolution effectResolution);

		const ResolutionScale& getEffectResolution() const;
		const ResolutionScale& getBlurResolution() const;

		virtual std::string toString() const;

	protected:
		virtual void drawSubImages();

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