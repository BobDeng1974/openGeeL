#include "guiwrapper.h"
#include "postprocessing/gaussianblur.h"
#include "postprocessing/motionblur.h"
#include "blursnippets.h"

namespace geeL {

	GaussianBlurSnippet::GaussianBlurSnippet(GaussianBlurBase& blur) : PostEffectSnippet(blur), blur(blur) {}

	void GaussianBlurSnippet::drawSimple(GUIContext * context) {
		float sigma = GUISnippets::drawBarFloatLogarithmic(context, blur.getSigma(), 0.1f, 25.f, 0.001f, "Sigma");
		blur.setSigma(sigma);

	}


	SeparatedGaussianSnippet::SeparatedGaussianSnippet(SeparatedGaussian& blur) 
		: GaussianBlurSnippet(blur), blur(blur){}

	void SeparatedGaussianSnippet::drawSimple(GUIContext * context) {
		float sigma = GUISnippets::drawBarFloatLogarithmic(context, blur.getSigmaR(), 0.0f, 25.f, 0.001f, "R");
		blur.setSigmaR(sigma);

		sigma = GUISnippets::drawBarFloatLogarithmic(context, blur.getSigmaG(), 0.0f, 25.f, 0.001f, "G");
		blur.setSigmaG(sigma);

		sigma = GUISnippets::drawBarFloatLogarithmic(context, blur.getSigmaB(), 0.0f, 25.f, 0.001f, "B");
		blur.setSigmaB(sigma);

		sigma = GUISnippets::drawBarFloat(context, blur.getFalloff(), 0.0f, 1.f, 0.001f, "Falloff");
		blur.setFalloff(sigma);
	}



	BilateralFilterSnippet::BilateralFilterSnippet(BilateralFilter& blur) 
		: GaussianBlurSnippet(blur), blur(blur) {}

	void BilateralFilterSnippet::drawSimple(GUIContext* context) {
		GaussianBlurSnippet::drawSimple(context);

		float sigma2 = GUISnippets::drawBarFloat(context, blur.getFactor(), 0.0f, 1.f, 0.001f, "Factor");
		blur.setFactor(sigma2);
	}



	SobelBlurSnippet::SobelBlurSnippet(SobelBlur& blur) : GaussianBlurSnippet(blur), blur(blur) {}

	void SobelBlurSnippet::drawSimple(GUIContext* context) {
		GaussianBlurSnippet::drawSimple(context);

		float scale = GUISnippets::drawBarFloat(context, blur.getScale(), 0.0f, 10.f, 0.001f, "Sobel Scale");
		blur.setScale(scale);
	}



	MotionBlurSnippet::MotionBlurSnippet(MotionBlur& blur) : PostEffectSnippet(blur), blur(blur) {}

	void MotionBlurSnippet::drawSimple(GUIContext * context) {
		float strength = GUISnippets::drawBarFloat(context, blur.getStrength(), 0.f, 1.f, 0.001f, "Strength");
		blur.setStrength(strength);

		int samples = GUISnippets::drawBarInteger(context, blur.getLevelOfDetail(), 1, 30, 1, "Details");
		blur.setLevelOfDetail(samples);
	}

}