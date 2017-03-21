#ifndef SIMPLEBLUR_H
#define SIMPLEBLUR_H

#include "postprocessing.h"

namespace geeL {

	//Simple (evenly distributed) blur that only uses one color channel (red)
	class SimpleBlur : public PostProcessingEffect {

	public:
		SimpleBlur(unsigned int strength = 1);

		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

	protected:
		SimpleBlur(unsigned int strength, std::string shaderPath);

	private:
		unsigned int amount;

	};


	//Simple (evenly distributed) blur that uses all color channels
	class SimpleBlur3D : public SimpleBlur {

	public:
		SimpleBlur3D(unsigned int strength = 1);

	};
}

#endif