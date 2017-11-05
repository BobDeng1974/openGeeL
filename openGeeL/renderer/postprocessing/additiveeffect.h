#ifndef ADDITIVEEFFECT_H
#define ADDITIVEEFFECT_H

#include "postprocessing.h"

namespace geeL {

	//Post effects that can draw into its texture target but cannot read existing values
	class AdditiveEffect : public PostProcessingEffectFS {

	public:
		AdditiveEffect(const std::string& fragmentPath);
		AdditiveEffect(const std::string& vertexPath, const std::string& fragmentPath);

		virtual void setImage(const Texture& texture);

		virtual void draw();
		virtual void fill();

	};

}

#endif

