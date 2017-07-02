#ifndef BLURSNIPPETS_H
#define BLURSNIPPETS_H

#include "guisnippets.h"
#include "postsnippets.h"

namespace geeL {

	class BilateralFilter;
	class GaussianBlur;
	class SobelBlur;


	class BilateralFilterSnippet : public PostEffectSnippet {

	public:
		BilateralFilterSnippet(BilateralFilter& blur);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		BilateralFilter& blur;

	};


	class SobelBlurSnippet : public PostEffectSnippet {

	public:
		SobelBlurSnippet(SobelBlur& blur);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		SobelBlur& blur;

	};


}

#endif
