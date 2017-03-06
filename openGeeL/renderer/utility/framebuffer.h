#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glew.h>
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

	struct FrameBufferInformation {

	public:
		unsigned int fbo;
		unsigned int width;
		unsigned int height;

	};


	class FrameBuffer {

	public:
		FrameBufferInformation info;

		FrameBuffer();

		void init(int width, int height, int colorBufferAmount = 1, ColorBufferType colorBufferType = RGBA16, 
			unsigned int filterMode = GL_LINEAR, bool useDepth = true);

		void init(int width, int height, int colorBufferAmount, std::vector<ColorBufferType> bufferTypes, 
			unsigned int filterMode = GL_LINEAR, bool useDepth = true);
	
		void fill(Drawer& drawer, bool setFBO = true) const;
		void copyDepth(unsigned int targetFBO) const;
		unsigned int getColorID(unsigned int position = 0) const;

		void bind() const;

		static void bind(unsigned int fbo);
		static void unbind();
		static void resetSize(int width, int height);
		void resize(int width, int height);

		unsigned int getFBO() const;
		unsigned int getWidth() const;
		unsigned int getHeight() const;


	private:
		unsigned int rbo;
		std::vector<unsigned int> colorBuffers;

		unsigned int generateTexture(ColorBufferType colorBufferType, unsigned int filterMode);
	};
}

#endif
