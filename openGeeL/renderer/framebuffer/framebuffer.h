#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <functional>
#include <string>
#include "../utility/resolution.h"
#include "../texturing/texture.h"
#include "../texturing/rendertexture.h"

namespace geeL {

	class Drawer;

	struct FrameBufferInformation {

	public:
		Resolution currResolution;

	};


	//Interface for all framebuffer objects
	class IFrameBuffer {

	public:
		virtual void add(RenderTexture& texture) {}
		virtual void fill(std::function<void()> drawCall) = 0;
		virtual void fill(Drawer& drawer) = 0;

		virtual void bind() const = 0;

		virtual void resetSize() const = 0;
		virtual const Resolution& getResolution() const = 0;

	protected:
		IFrameBuffer() {}

	private:
		IFrameBuffer(const IFrameBuffer& other) = delete;
		IFrameBuffer& operator= (const IFrameBuffer& other) = delete;

	};


	//Abstract base class for all framebuffer objects
	class FrameBuffer : public IFrameBuffer {

	public:
		FrameBuffer() {}

		virtual void fill(std::function<void()> drawCall) = 0;
		virtual void fill(Drawer& drawer);

		virtual void bind() const;
		static void unbind();

		virtual void copyDepth(const FrameBuffer& buffer) const;
		
		virtual const Resolution& getResolution() const;
		virtual void resetSize() const;
		static void resetSize(Resolution resolution);
		void remove();
		
		virtual bool initialized() const;
		virtual std::string toString() const = 0;

	protected:
		unsigned int fbo;
		Resolution resolution;
		FrameBufferInformation info;

		unsigned int getFBO() const;
		void clear() const;

	private:
		FrameBuffer(const FrameBuffer& other) = delete;
		FrameBuffer& operator= (const FrameBuffer& other) = delete;
		
	};


	inline bool FrameBuffer::initialized() const {
		return fbo != 0;
	}


}

#endif
