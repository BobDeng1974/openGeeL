#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>

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
	
	int width;
	int height;

	FrameBuffer();

	void init(int width, int height, int colorBufferAmount = 1, bool useDepth = true, 
		ColorBufferType colorBufferType = RGBA16, unsigned int filterMode = GL_LINEAR);
	
	void fill(Drawer& drawer) const;
	void copyDepth(unsigned int targetFBO) const;
	unsigned int getColorID(unsigned int position = 0) const;

	void bind() const;

	static void bind(unsigned int fbo);
	static void unbind();

private:
	std::vector<unsigned int> colorBuffers;

	unsigned int generateTexture(ColorBufferType colorBufferType, unsigned int filterMode);

};

}

#endif
