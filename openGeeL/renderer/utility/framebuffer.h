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

	FrameBuffer();

	void init(int width, int height);
	void fill(Drawer& drawer);
	void copyDepth(unsigned int targetFBO);

private:
	unsigned int generateTexture(bool color);

};

}

#endif
