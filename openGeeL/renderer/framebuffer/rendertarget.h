#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "utility/resolution.h"

namespace geeL {

	class IFrameBuffer;

	//Interface for all objects that can be used as drawing target of a framebuffer
	class RenderTarget {

	public:
		//Assing this target to given framebuffer
		virtual void assignTo(const IFrameBuffer& buffer, unsigned int position, bool bindFB = false) = 0;

		//Assign this target to given framebuffer besides an already assigned main buffer. 
		//Note: This function will only work if target has been assigned to a main buffer beforehand
		virtual bool assignToo(const IFrameBuffer& buffer, unsigned int position, bool bindFB = false) const = 0;

		virtual void setRenderResolution() const = 0;
		virtual const Resolution& getResolution() const = 0;

	};

}

#endif