#ifndef SCREENQUAD_H
#define SCREENQUAD_H

#include "primitive.h"

namespace geeL {

	//Screen for post processing draw calls or deferred shading
	class ScreenQuad : public Primitive {

	public:
		virtual void init();
		virtual void draw() const;

	};
}

#endif
