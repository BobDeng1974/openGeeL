#ifndef RENDERCONTEXT_H
#define RENDERCONTEXT_H

#define ENABLE_STENCIL 1

namespace geeL {

	//Object that abstract rendering options like blending
	class RenderContext {

	public:
		RenderContext();

		void reset();

	};
}

#endif