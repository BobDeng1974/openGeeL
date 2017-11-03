#define GLEW_STATIC
#include <glew.h>
#include "clearing.h"

namespace geeL {

	void Clearer::clear() const {
		if (i == 0) return;

		if ((i & (unsigned int)ClearingMethod::Color) != 0) {
			glClear(GL_COLOR_BUFFER_BIT);
		}
			
		if ((i & (unsigned int)ClearingMethod::Depth) != 0) {
			glClear(GL_DEPTH_BUFFER_BIT);
		}
			
		if ((i & (unsigned int)ClearingMethod::Stencil) != 0) {
			glClear(GL_STENCIL_BUFFER_BIT);
		}

	}

	

}