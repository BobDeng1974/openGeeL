#include "guiwrapper.h"
#include "postprocessing/gaussianblur.h"
#include "postprocessing/motionblur.h"
#include "blursnippets.h"

namespace geeL {

	GaussianBlurSnippet::GaussianBlurSnippet(GaussianBlurBase& blur) : PostEffectSnippet(blur), blur(blur) {}

	void GaussianBlurSnippet::draw(GUIContext * context) {
		float sigma = GUISnippets::drawBarFloat(context, blur.getSigma(), 0.1f, 15.f, 0.001f, "Sigma");
		blur.setSigma(sigma);

	}



	BilateralFilterSnippet::BilateralFilterSnippet(BilateralFilter& blur) 
		: GaussianBlurSnippet(blur), blur(blur) {}

	void BilateralFilterSnippet::draw(GUIContext* context) {
		GaussianBlurSnippet::draw(context);

		float sigma = GUISnippets::drawBarFloat(context, blur.getSigma(), 0.0f, 1.f, 0.001f, "Factor");
		blur.setSigma(sigma);
	}



	SobelBlurSnippet::SobelBlurSnippet(SobelBlur& blur) : GaussianBlurSnippet(blur), blur(blur) {}

	void SobelBlurSnippet::draw(GUIContext* context) {
		GaussianBlurSnippet::draw(context);

		float scale = GUISnippets::drawBarFloat(context, blur.getScale(), 0.0f, 10.f, 0.001f, "Sobel Scale");
		blur.setScale(scale);
	}



	MotionBlurSnippet::MotionBlurSnippet(MotionBlur& blur) : PostEffectSnippet(blur), blur(blur) {}

	void MotionBlurSnippet::draw(GUIContext * context) {
		float strength = GUISnippets::drawBarFloat(context, blur.getStrength(), 0.f, 1.f, 0.001f, "Strength");
		blur.setStrength(strength);

		int samples = GUISnippets::drawBarInteger(context, blur.getLevelOfDetail(), 1, 30, 1, "Details");
		blur.setLevelOfDetail(samples);
	}

}