#ifndef TONEMAPPING_H
#define TONEMAPPING_H

#include "postprocessing.h"

namespace geeL {

	//Simple Reinhardt tone mapping
	class ToneMapping : public PostProcessingEffectFS {

	public:
		ToneMapping(float exposure);

		virtual void init(const PostProcessingParameter& parameter);

	protected:
		float exposure;

	};
}

#endif

