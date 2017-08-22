#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <functional>
#include <string>
#include "utility/resolution.h"
#include "texturing/texture.h"
#include "texturing/rendertexture.h"
#include "fbotoken.h"

namespace geeL {

	class Drawer;


	//Interface for all framebuffer objects
	class IFrameBuffer {

	public:
		virtual void add(RenderTexture& texture) {} //TODO: remove this later on

		virtual void fill(std::function<void()> drawCall) = 0;
		virtual void fill(Drawer& drawer) = 0;

		virtual void bind() const = 0;

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

		virtual void fill(std::function<void()> drawCall) = 0;
		virtual void fill(Drawer& drawer);

		virtual void bind() const;
		static void bind(unsigned int fbo);
		static void unbind();

		virtual void copyDepth(const FrameBuffer& buffer) const;
		
		virtual const Resolution& getResolution() const;
		virtual void resetSize() const;
		static void resetSize(Resolution resolution);
		
		virtual bool initialized() const;
		virtual std::string toString() const = 0;

	protected:
		FrameBufferToken fbo;
		Resolution resolution;

		unsigned int getFBO() const;
		void clear() const;

	private:
		FrameBuffer(const FrameBuffer& other) = delete;
		FrameBuffer& operator= (const FrameBuffer& other) = delete;
		
	};


	//Framebuffer that allows adding and filling of render textures during runtime
	class DynamicBuffer : public FrameBuffer {

	public:
		virtual void add(RenderTexture& texture) = 0;

		//Returns current render texture of this buffer or null pointer
		//if no texture is attached
		virtual const RenderTexture* const getTexture() const = 0;

	};



	inline bool FrameBuffer::initialized() const {
		return fbo != 0;
	}


}

#endif
