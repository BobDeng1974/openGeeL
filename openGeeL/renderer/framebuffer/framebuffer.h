#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <functional>
#include <string>
#include "utility/resolution.h"
#include "glwrapper/clearing.h"
#include "texturing/texture.h"
#include "fbotoken.h"

namespace geeL {

	class Drawer;
	class RenderTarget;


	//Interface for all framebuffer objects
	class IFrameBuffer {

	public:
		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNormal) = 0;
		virtual void fill(Drawer& drawer, Clearer clearer = clearNormal);

		void bind() const;
		static void bind(unsigned int fbo);
		static void unbind();

		virtual void applyRenderResolution() const;
		virtual Resolution getResolution() const = 0;

	protected:
		IFrameBuffer() {}
		virtual ~IFrameBuffer() {};

		virtual unsigned int getFBO() const = 0;

	private:
		static unsigned int activeFBO;

		IFrameBuffer(const IFrameBuffer& other) = delete;
		IFrameBuffer& operator= (const IFrameBuffer& other) = delete;

	};


	//Abstract base class for all framebuffer objects
	class FrameBuffer : public IFrameBuffer {

	public:
		FrameBuffer() {}
		virtual ~FrameBuffer() {}

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNormal) = 0;
		using IFrameBuffer::fill;

		//Use RBO of given framebuffer
		//Note: RBO will be referenced and not copied. 
		//Therefore, their behaviour will affect each other
		void referenceRBO(FrameBuffer& buffer);
		void referenceRBO(const RenderBufferToken& rbo);

		//Copy contents of buffers RBO into this buffers RBO
		void copyRBO(FrameBuffer& buffer);

		virtual void copyDepth(const FrameBuffer& buffer) const;
		virtual void copyStencil(const FrameBuffer& buffer) const;
		
		virtual bool initialized() const;
		virtual std::string toString() const = 0;

	protected:
		RenderBufferToken rbo;
		FrameBufferToken fbo;

		virtual unsigned int getFBO() const;

	private:
		FrameBuffer(const FrameBuffer& other) = delete;
		FrameBuffer& operator= (const FrameBuffer& other) = delete;
		
	};


	//Framebuffer that allows adding and filling of render textures during runtime
	class DynamicBuffer : public FrameBuffer {

	public:
		virtual void add(RenderTarget& target);
		virtual void push(RenderTarget& target) = 0;
	};



	inline bool FrameBuffer::initialized() const {
		return fbo != 0;
	}

	inline void DynamicBuffer::add(RenderTarget& texture) {
		push(texture);
	}

}

#endif
