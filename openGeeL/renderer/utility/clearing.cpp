#define GLEW_STATIC
#include <glew.h>
#include "clearing.h"

#include <iostream>

namespace geeL {

	void Clearer::clear() const {
		if (i == 0) return;

		unsigned int clear = ((i & (unsigned int)ClearingMethod::Color) != 0) * GL_COLOR_BUFFER_BIT
			+ ((i & (unsigned int)ClearingMethod::Depth) != 0) * GL_DEPTH_BUFFER_BIT
			+ ((i & (unsigned int)ClearingMethod::Stencil) != 0) * GL_STENCIL_BUFFER_BIT;

		glClear(clear);
	}

}