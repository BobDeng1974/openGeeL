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
		glStencilFunc(GL_EQUAL, (int)mask, (int)mask);
#endif
	}

	void Masking::passthrough() {
#if ENABLE_STENCIL
		glStencilMask(0x00);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
#endif
	}


	RenderMask Masking::getShadingMask(ShadingMethod method) {

		switch (method) {
			case ShadingMethod::Forward:
				return RenderMask::Forward;
			case ShadingMethod::TransparentOD:
				return RenderMask::Transparent;
			case ShadingMethod::Generic:
				return RenderMask::Generic;
		}

		return RenderMask::None;
	}

}