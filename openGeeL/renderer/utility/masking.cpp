#define GLEW_STATIC
#include <glew.h>
#include "masking.h"

namespace geeL {


	void Masking::drawMask(RenderMask mask) {
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, (int)mask, 0xFF);

	}

	void Masking::readMask(RenderMask mask) {
		glStencilMask(0x00);
		glStencilFunc(GL_EQUAL, (int)mask, 0xFF);

	}

}