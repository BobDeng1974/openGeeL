#ifndef SIMPLEBLUR_H
#define SIMPLEBLUR_H

#include "postprocessing.h"

namespace geeL {

	//Simple (evenly distributed) blur that only uses one color channel (red)
	class SimpleBlur : public PostProcessingEffectFS {

	public:
		SimpleBlur(unsigned int strength = 1);

		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution);
		virtual std::string toString() const;

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


	inline std::string SimpleBlur::toString() const {
		return "Simple Blur";
	}

}

#endif