#include "../guiwrapper.h"
#include "../../renderer/postprocessing/gaussianblur.h"
#include "../../renderer/postprocessing/motionblur.h"
#include "blursnippets.h"

namespace geeL {

	GaussianBlurSnippet::GaussianBlurSnippet(GaussianBlur & blur) : blur(blur) {}

	void GaussianBlurSnippet::draw(GUIContext * context) {
		float sigma = GUISnippets::drawBarFloat(context, blur.getSigma(), 0.1f, 15.f, 0.001f, "Sigma");
		blur.setSigma(sigma);

	}

	std::string GaussianBlurSnippet::toString() const {
		return "Gaussian Blur";
	}


	BilateralFilterSnippet::BilateralFilterSnippet(BilateralFilter& blur) 
		: GaussianBlurSnippet(blur), blur(blur) {}

	void BilateralFilterSnippet::draw(GUIContext* context) {
		GaussianBlurSnippet::draw(context);

		float sigma = GUISnippets::drawBarFloat(context, blur.getSigma(), 0.0f, 1.f, 0.001f, "Factor");
		blur.setSigma(sigma);
	}

	std::string BilateralFilterSnippet::toString() const {
		return "Bilateral Filter";
	}


	SobelBlurSnippet::SobelBlurSnippet(SobelBlur& blur) 
		: GaussianBlurSnippet(blur), blur(blur) {}

	void SobelBlurSnippet::draw(GUIContext* context) {
		GaussianBlurSnippet::draw(context);

		float scale = GUISnippets::drawBarFloat(context, blur.getScale(), 0.0f, 10.f, 0.001f, "Sobel Scale");
		blur.setScale(scale);
	}

	std::string SobelBlurSnippet::toString() const {
		return "Sobel Blur";
	}

	MotionBlurSnippet::MotionBlurSnippet(MotionBlur& blur) : blur(blur) {}

	void MotionBlurSnippet::draw(GUIContext * context) {
		float strength = GUISnippets::drawBarFloat(context, blur.getStrength(), 0.f, 1.f, 0.001f, "Strength");
		blur.setStrength(strength);

	}

	std::string MotionBlurSnippet::toString() const {
		return "Motion Blur";
	}

}