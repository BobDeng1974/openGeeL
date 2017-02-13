#ifndef FXAA_H
#define FXAA_H

#include "postprocessing.h"

namespace geeL {

	//Fast Approximate Anti-Aliasing post effect
	class FXAA : public PostProcessingEffect {

	public:
		FXAA() : PostProcessingEffect("renderer/postprocessing/fxaa.frag") {}

	};
}

#endif

