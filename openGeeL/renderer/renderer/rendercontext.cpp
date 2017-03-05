#define GLEW_STATIC
#include <glew.h>
#include "rendercontext.h"

namespace geeL {

	RenderContext::RenderContext() {

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
	}


	void RenderContext::reset() {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

}
