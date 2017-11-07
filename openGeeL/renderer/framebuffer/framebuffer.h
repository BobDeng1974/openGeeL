#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <functional>
#include <string>
#include "utility/resolution.h"
#include "utility/clearing.h"
#include "texturing/texture.h"
#include "texturing/rendertexture.h"
#include "fbotoken.h"

namespace geeL {

	class Drawer;


	//Interface for all framebuffer objects
	class IFrameBuffer {

	public:
		virtual void add(RenderTexture& texture) {}

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNormal) = 0;
		virtual void fill(Drawer& drawer, Clearer clearer = clearNormal) = 0;

		virtual void bind() const = 0;
		static void unbind();

		virtual void resetSize() const = 0;
		virtual const Resolution& getResolution() const = 0;

	protected:
		IFrameBuffer() {}
		virtual ~IFrameBuffer() {};

	private:
		IFrameBuffer(const IFrameBuffer& other) = delete;
		IFrameBuffer& operator= (const IFrameBuffer& other) = delete;

	};



	//Abstract base class for all framebuffer objects
	class FrameBuffer : public IFrameBuffer {

	public:
		FrameBuffer() {}
		virtual ~FrameBuffer() {}

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNormal) = 0;
		virtual void fill(Drawer& drawer, Clearer clearer = clearNormal);

		virtual void bind() const;
		static void bind(unsigned int fbo);

		//Use RBO of given framebuffer
		//Note: RBO will be referenced and not copied. 
		//Therefore, their behaviour will affect each other
		void referenceRBO(FrameBuffer& buffer);
		void referenceRBO(const RenderBufferToken& rbo);

		//Copy contents of buffers RBO into this buffers RBO
		void copyRBO(FrameBuffer& buffer);

		virtual void copyDepth(const FrameBuffer& buffer) const;
		virtual void copyStencil(const FrameBuffer& buffer) const;
		
		virtual const Resolution& getResolution() const;
		virtual void resetSize() const;
		static void resetSize(Resolution resolution);
		
		virtual bool initialized() const;
		virtual std::string toString() const = 0;

	protected:
		RenderBufferToken rbo;
		FrameBufferToken fbo;
		Resolution resolution;

		unsigned int getFBO() const;

	private:
		FrameBuffer(const FrameBuffer& other) = delete;
		FrameBuffer& operator= (const FrameBuffer& other) = delete;
		
	};


	//Framebuffer that allows adding and filling of render textures during runtime
	class DynamicBuffer : public FrameBuffer {

	public:
		virtual void add(RenderTexture& texture);
		virtual void push(RenderTexture& texture) = 0;
		virtual void pop() = 0;

		//Returns current render texture of this buffer or null pointer
		//if no texture is attached
		virtual const RenderTexture* const getTexture() const = 0;

	};



	inline bool FrameBuffer::initialized() const {
		return fbo != 0;
	}

	inline void DynamicBuffer::add(RenderTexture& texture) {
		push(texture);
	}


}

#endif
