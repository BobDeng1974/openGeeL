#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

namespace geeL {

class Drawer;

class FrameBuffer {

public:
	unsigned int fbo;
	unsigned int color;
	unsigned int depth;
	int width;
	int height;

	FrameBuffer(int width, int height);

	void init();
	void fill(Drawer& drawer);

private:
	unsigned int generateTexture(bool color);

};

}

#endif
