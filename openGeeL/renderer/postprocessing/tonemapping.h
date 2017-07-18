#ifndef TONEMAPPING_H
#define TONEMAPPING_H

#include "postprocessing.h"

namespace geeL {

	//Simple Reinhardt tone mapping
	class ToneMapping : public PostProcessingEffect {

	public:
		ToneMapping(float exposure);

		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution);

	protected:
		float exposure;

	};
}

#endif

