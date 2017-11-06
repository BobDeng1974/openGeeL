#ifndef GAMMACORRECTION_H
#define GAMMACORRECTION_H

#include "postprocessing.h"

namespace geeL {

	class GammaCorrection : public PostProcessingEffectFS {

	public:
		GammaCorrection(float gamma = 2.2f);

		virtual void init(const PostProcessingParameter& parameter);

	private:
		float gamma;
	};
}

#endif
