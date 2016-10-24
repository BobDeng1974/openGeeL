#ifndef BLOOM_H
#define BLOOM_H

#include "../utility/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

class FrameBuffer;
class GaussianBlur;

class BloomFilter : public PostProcessingEffect {

public:
	float scatter;

	BloomFilter(float scatter);

protected:
	virtual void bindValues();

};


class Bloom : public PostProcessingEffect {

public:
	Bloom(GaussianBlur& blur, float scatter = 0.6f);
	~Bloom();

	void setScatter(float scatter);
	virtual void setScreen(PostProcessingScreen& screen);

protected:
	virtual void bindValues();

private:
	BloomFilter* filter;
	GaussianBlur& blur;
	FrameBuffer filterBuffer;
	FrameBuffer blurBuffer;

};


}

#endif
