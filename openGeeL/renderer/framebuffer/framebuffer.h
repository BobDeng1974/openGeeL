#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <functional>
#include <glew.h>
#include <vector>
#include <string>
#include "../utility/resolution.h"
#include "../texturing/texture.h"
#include "../texturing/rendertexture.h"

namespace geeL {

	class Drawer;

	struct FrameBufferInformation {

	public:
		unsigned int fbo;
		unsigned int currWidth;
		unsigned int currHeight;
		unsigned int baseWidth;
		unsigned int baseHeight;

		void setDimension(unsigned int width, unsigned int height) {
			currWidth = baseWidth = width;
			currHeight = baseHeight = height;
		}

	};


	//Abstract base class for all framebuffer objects
	class FrameBuffer {

	public:
		FrameBuffer() {}

		virtual void fill(std::function<void()> drawCall) const = 0;
		virtual void fill(std::function<void()> drawCall);
		virtual void fill(Drawer& drawer) const;

		void bind() const;
		static void unbind();

		virtual void copyDepth(const FrameBuffer& buffer) const;
		
		void resetSize() const;
		static void resetSize(int width, int height);
		void remove();
		
		unsigned int getWidth() const;
		unsigned int getHeight() const;

		virtual std::string toString() const = 0;

	protected:
		FrameBufferInformation info;

		unsigned int getFBO() const;

	private:
		FrameBuffer(const FrameBuffer& other) = delete;
		FrameBuffer& operator= (const FrameBuffer& other) = delete;
		
	};


	//A simple framebuffer that can store 2D textures
	class ColorBuffer : public FrameBuffer {

	public:
		ColorBuffer() {}
		~ColorBuffer();

		void init(unsigned int width, unsigned int height, std::vector<RenderTexture*>&& colorBuffers, bool useDepth = true);
		void init(unsigned int width, unsigned int height, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::None, WrapMode wrapMode = WrapMode::ClampEdge, bool useDepth = true);

		virtual void fill(std::function<void()> drawCall) const;
		virtual void fill(Drawer& drawer) const;

		const RenderTexture& getTexture(unsigned int position = 0) const;
		virtual void resize(Resolution resolution);

		virtual std::string toString() const;

	private:
		unsigned int rbo;
		std::vector<RenderTexture*> buffers;

		ColorBuffer(const ColorBuffer& other) = delete;
		ColorBuffer& operator= (const ColorBuffer& other) = delete;

	};
}

#endif
