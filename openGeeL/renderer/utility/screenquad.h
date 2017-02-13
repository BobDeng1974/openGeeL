#ifndef SCREENQUAD_H
#define SCREENQUAD_H

namespace geeL {

	//Screen for post processing draw calls or deferred shading
	class ScreenQuad {

	public:
		unsigned int vbo;
		unsigned int vao;
		int width;
		int height;

		ScreenQuad(int width, int height)
			: width(width), height(height) {}

		void init();
		void draw() const;

	};
}

#endif
