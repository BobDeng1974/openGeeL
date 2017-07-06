#ifndef BLURSNIPPETS_H
#define BLURSNIPPETS_H

#include "guisnippets.h"
#include "postsnippets.h"

namespace geeL {

	class BilateralFilter;
	class GaussianBlur;
	class MotionBlur;
	class SobelBlur;


	class GaussianBlurSnippet : public PostEffectSnippet {

	public:
		GaussianBlurSnippet(GaussianBlur& blur);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		GaussianBlur& blur;

	};


	class BilateralFilterSnippet : public GaussianBlurSnippet {

	public:
		BilateralFilterSnippet(BilateralFilter& blur);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		BilateralFilter& blur;

	};


	class SobelBlurSnippet : public GaussianBlurSnippet {

	public:
		SobelBlurSnippet(SobelBlur& blur);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		SobelBlur& blur;

	};


	class MotionBlurSnippet : public PostEffectSnippet {

	public:
		MotionBlurSnippet(MotionBlur& blur);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		MotionBlur& blur;

	};

}

#endif
