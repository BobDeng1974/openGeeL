#ifndef SCREENQUAD_H
#define SCREENQUAD_H

namespace geeL {

	//Screen for post processing draw calls or deferred shading
	class ScreenQuad {

	public:
		unsigned int vbo;
		unsigned int vao;

		void init();
		void draw() const;

	};
}

#endif
