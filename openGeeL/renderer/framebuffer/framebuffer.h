#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <functional>
#include <glew.h>
#include <vector>
#include <string>
#include "../texturing/texture.h"
#include "../texturing/rendertexture.h"

namespace geeL {

	class Drawer;

	struct FrameBufferInformation {

	public:
		unsigned int fbo;
		unsigned int width;
		unsigned int height;

	};


	//Abstract base class for all framebuffer objects
	class FrameBuffer {

	public:
		FrameBufferInformation info;

		FrameBuffer() {}
		FrameBuffer(const FrameBuffer& buffer);

		virtual void fill(std::function<void()> drawCall) = 0;

		void bind() const;
		static void bind(unsigned int fbo);
		static void unbind();

		void copyDepth(unsigned int targetFBO) const;
		void copyDepth(const FrameBuffer& buffer) const;
		
		static void resetSize(int width, int height);
		static void remove(unsigned int fbo);
		void remove();

		unsigned int getFBO() const;
		unsigned int getWidth() const;
		unsigned int getHeight() const;

		virtual std::string toString() const = 0;

	};


	//A simple framebuffer that can store 2D textures
	class ColorBuffer : public FrameBuffer {

	public:
		ColorBuffer() {}
		ColorBuffer(const ColorBuffer& buffer);
		~ColorBuffer();

		void init(unsigned int width, unsigned int height, std::vector<RenderTexture*>&& colorBuffers, bool useDepth = true);

		void init(unsigned int width, unsigned int height, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::Nearest, WrapMode wrapMode = WrapMode::ClampBorder, bool useDepth = true);

		virtual void fill(std::function<void()> drawCall);
		void fill(Drawer& drawer, bool setFBO = true) const;

		const RenderTexture& getTexture(unsigned int position = 0) const;
		
		void resize(int width, int height);

		virtual std::string toString() const;

	private:
		unsigned int rbo;
		std::vector<RenderTexture*> buffers;

	};
}

#endif
