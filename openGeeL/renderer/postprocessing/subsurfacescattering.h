#ifndef SUBSURFACESCATTERING_H
#define SUBSURFACESCATTERING_H

#include "gaussianblur.h"

namespace geeL {

	class SubsurfaceScattering : public SeparatedGaussian {

	public:
		SubsurfaceScattering(float sigma = 1.3f, 
			unsigned int kernelSize = 7, 
			float falloff = 0.f);


		virtual void init(const PostProcessingParameter& parameter);

		float getFalloff() const;
		void setFalloff(float value);

	private:
		float falloff;

	};

}

#endif