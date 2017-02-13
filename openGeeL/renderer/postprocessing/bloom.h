#ifndef BLOOM_H
#define BLOOM_H

#include "../utility/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

	class FrameBuffer;
	class GaussianBlur;
	class ScreenQuad;

	class BloomFilter : public PostProcessingEffect {

	public:
		float scatter;

		BloomFilter(float scatter);

	protected:
		virtual void bindValues();

	};


	class Bloom : public PostProcessingEffect {

	public:
		Bloom(GaussianBlur& blur, float scatter = 0.6f, float blurResolution = 1.f);
		~Bloom();

		void setScatter(float scatter);
		virtual void setScreen(ScreenQuad& screen);

	protected:
		virtual void bindValues();

	private:
		float blurResolution;

		BloomFilter* filter;
		GaussianBlur& blur;
		FrameBuffer filterBuffer;
		FrameBuffer blurBuffer;
		ScreenQuad* blurScreen;

	};
}

#endif
