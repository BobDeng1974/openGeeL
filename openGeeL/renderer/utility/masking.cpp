#define GLEW_STATIC
#include <glew.h>
#include "renderer/rendercontext.h"
#include "masking.h"

namespace geeL {


	void Masking::drawMask(RenderMask mask) {
#if ENABLE_STENCIL
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, (int)mask, 0xFF);
#endif
	}

	void Masking::readMask(RenderMask mask) {
#if ENABLE_STENCIL
		glStencilMask(0x00);
		glStencilFunc(GL_EQUAL, (int)mask, 0xFF);
#endif
	}

	void Masking::passthrough() {
#if ENABLE_STENCIL
		glStencilMask(0x00);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
#endif
	}

}