#ifndef SCREENQUAD_H
#define SCREENQUAD_H

#include "primitive.h"

namespace geeL {

	//Screen for post processing draw calls or deferred shading
	class ScreenQuad : public Primitive {

	public:
		virtual void init();
		virtual void draw() const;

		//Returns the default screen quad
		//Note: It is not guaranteed that this primitive
		//has been initialized
		static ScreenQuad& get();

	};

}

#endif
