#define GLEW_STATIC
#include <glew.h>
#include "clearing.h"

namespace geeL {

	void Clearer::clear() const {
		if (i == 0) return;

		bool clearStencil = (i & (unsigned int)ClearingMethod::Stencil) != 0;

		unsigned int clear = ((i & (unsigned int)ClearingMethod::Color) != 0) * GL_COLOR_BUFFER_BIT
			+ ((i & (unsigned int)ClearingMethod::Depth) != 0) * GL_DEPTH_BUFFER_BIT
			+ clearStencil * GL_STENCIL_BUFFER_BIT;

		if(clearStencil) glStencilMask(0xFF);
		glClear(clear);
	}

	void Clearer::setClearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}

}