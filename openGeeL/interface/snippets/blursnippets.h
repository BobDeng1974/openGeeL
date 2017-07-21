#ifndef BLURSNIPPETS_H
#define BLURSNIPPETS_H

#include "guisnippets.h"
#include "postsnippets.h"

namespace geeL {

	class BilateralFilter;
	class GaussianBlurBase;
	class MotionBlur;
	class SobelBlur;


	class GaussianBlurSnippet : public PostEffectSnippet {

	public:
		GaussianBlurSnippet(GaussianBlurBase& blur);

		virtual void draw(GUIContext* context);

	private:
		GaussianBlurBase& blur;

	};


	class BilateralFilterSnippet : public GaussianBlurSnippet {

	public:
		BilateralFilterSnippet(BilateralFilter& blur);

		virtual void draw(GUIContext* context);

	private:
		BilateralFilter& blur;

	};


	class SobelBlurSnippet : public GaussianBlurSnippet {

	public:
		SobelBlurSnippet(SobelBlur& blur);

		virtual void draw(GUIContext* context);

	private:
		SobelBlur& blur;

	};


	class MotionBlurSnippet : public PostEffectSnippet {

	public:
		MotionBlurSnippet(MotionBlur& blur);

		virtual void draw(GUIContext* context);

	private:
		MotionBlur& blur;

	};

}

#endif
