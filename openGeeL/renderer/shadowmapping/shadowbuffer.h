#ifndef SHADOWBUFFER_H
#define SHADOWBUFFER_H

#include "framebuffer/framebuffer.h"

namespace geeL {

	class Texture;


	class DepthFrameBuffer : public IFrameBuffer {

	public:
		DepthFrameBuffer(const Resolution& defaultResolution);

		void add(Texture& texture);

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNothing);
		virtual const Resolution& getResolution() const;

	protected:
		virtual unsigned int getFBO() const;

	private:
		FrameBufferToken fbo;
		Resolution resolution;
		Resolution currResolution;
		Clearer clearer;
		Texture* current;

	};

}


#endif
