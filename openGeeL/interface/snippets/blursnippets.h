#ifndef BLURSNIPPETS_H
#define BLURSNIPPETS_H

#include "guisnippets.h"
#include "postsnippets.h"

namespace geeL {

	class BilateralFilter;
	class GaussianBlurBase;
	class SeparatedGaussian;
	class MotionBlur;
	class SobelBlur;


	class GaussianBlurSnippet : public PostEffectSnippet {

	public:
		GaussianBlurSnippet(GaussianBlurBase& blur);

		virtual void drawSimple(GUIContext* context);

	private:
		GaussianBlurBase& blur;

	};


	class SeparatedGaussianSnippet : public GaussianBlurSnippet {

	public:
		SeparatedGaussianSnippet(SeparatedGaussian& blur);

		virtual void drawSimple(GUIContext* context);

	private:
		SeparatedGaussian& blur;

	};


	class BilateralFilterSnippet : public GaussianBlurSnippet {

	public:
		BilateralFilterSnippet(BilateralFilter& blur);

		virtual void drawSimple(GUIContext* context);

	private:
		BilateralFilter& blur;

	};


	class SobelBlurSnippet : public GaussianBlurSnippet {

	public:
		SobelBlurSnippet(SobelBlur& blur);

		virtual void drawSimple(GUIContext* context);

	private:
		SobelBlur& blur;

	};


	class MotionBlurSnippet : public PostEffectSnippet {

	public:
		MotionBlurSnippet(MotionBlur& blur);

		virtual void drawSimple(GUIContext* context);

	private:
		MotionBlur& blur;

	};

}

#endif
