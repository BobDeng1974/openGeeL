#define GLEW_STATIC
#include <glew.h>
#include "glwrapper/glguards.h"
#include "appglobals.h"
#include "rendercontext.h"

namespace geeL {

	RenderContext::RenderContext() {

#if ENABLE_STENCIL
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
#endif
		DepthGuard::enableForced(true);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
	}


	void RenderContext::reset() {

		DepthGuard::enableForced(true);
		glEnable(GL_CULL_FACE);
	}

}
