#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glew.h>
#include <vector>
#include "../texturing/texture.h"

namespace geeL {

	class Drawer;


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

		void init(unsigned int width, unsigned int height, int colorBufferAmount = 1, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::Nearest, WrapMode wrapMode = WrapMode::ClampBorder, bool useDepth = true);

		void init(unsigned int width, unsigned int height, int colorBufferAmount, std::vector<ColorType> bufferTypes,
			FilterMode filterMode = FilterMode::Nearest, WrapMode wrapMode = WrapMode::ClampBorder, bool useDepth = true);
	
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

		unsigned int generateTexture(ColorType colorType, FilterMode filterMode, WrapMode wrapMode);
	};
}

#endif
