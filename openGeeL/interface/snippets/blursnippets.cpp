#include "../guiwrapper.h"
#include "../../renderer/postprocessing/gaussianblur.h"
#include "blursnippets.h"

namespace geeL {

	BilateralFilterSnippet::BilateralFilterSnippet(BilateralFilter& blur) : blur(blur) {}

	void BilateralFilterSnippet::draw(GUIContext* context) {
		float sigma = GUISnippets::drawBarFloat(context, blur.getSigma(), 0.0f, 1.f, 0.001f, "Sigma");
		blur.setSigma(sigma);

	}

	std::string BilateralFilterSnippet::toString() const {
		return "Bilateral Filter";
	}


	SobelBlurSnippet::SobelBlurSnippet(SobelBlur& blur) : blur(blur) {}

	void SobelBlurSnippet::draw(GUIContext* context) {
		float scale = GUISnippets::drawBarFloat(context, blur.getScale(), 0.0f, 50.f, 0.001f, "Sobel Scale");
		blur.setScale(scale);

	}

	std::string SobelBlurSnippet::toString() const {
		return "Sobel Blur";
	}


}