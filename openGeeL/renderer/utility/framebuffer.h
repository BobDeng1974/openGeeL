#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

namespace geeL {

class Drawer;


enum ColorBufferType {
	Single,
	RGB,
	RGBA,
	RGB16,
	RGBA16
};

class FrameBuffer {

public:
	unsigned int fbo;
	unsigned int color;
	int width;
	int height;

	FrameBuffer();

	void init(int width, int height, bool useDepth = true, 
		ColorBufferType colorBufferType = RGBA16, unsigned int filterMode = GL_LINEAR);
	
	void fill(Drawer& drawer);
	void copyDepth(unsigned int targetFBO);

private:
	unsigned int generateTexture(ColorBufferType colorBufferType, unsigned int filterMode);

};

}

#endif
