#ifndef BLOOM_H
#define BLOOM_H

#include "../utility/framebuffer.h"
#include "blurredeffect.h"
#include "postprocessing.h"

namespace geeL {

	class FrameBuffer;
	class GaussianBlur;
	class ScreenQuad;

	//Filter effect that filters bright colors / lights in image depending on given scatter
	class BloomFilter : public PostProcessingEffect {

	public:
		float scatter;

		BloomFilter(float scatter = 0.6f);

		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

		void setScatter(float scatter);
		float getScatter() const;

	};


	//Post effect that introduces light bleeding into the scene
	class Bloom : public BlurredPostEffect {

	public:
		Bloom(BloomFilter& filter, GaussianBlur& blur, 
			float effectResolution = 1.0f, float blurResolution = 1.f);

		void setScatter(float scatter);
		float getScatter() const;

	private:
		BloomFilter& filter;

	};
}

#endif
