#ifndef SCREENCUBE_H
#define SCREENCUBE_H

#include "primitive.h"

namespace geeL {

	//Screen for post processing draw calls or deferred shading
	class ScreenCube : public Primitive {

	public:
		virtual void init();
		virtual void draw() const;

	};

	static ScreenCube SCREENCUBE = ScreenCube();

}

#endif

