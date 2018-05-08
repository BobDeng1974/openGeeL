#ifndef PINGPONGBUFFER_H
#define PINGPONGBUFFER_H

#include "framebuffer.h"

namespace geeL {

	class RenderTarget;


	//Buffer that uses different textures to draw images in alternating order
	class PingPongBuffer : public FrameBuffer {

	public:
		PingPongBuffer();

		//Init buffer with external textures which will be filled by subsequent drawcalls
		//Note: Memory of render textures won't be managed by this color buffer
		//Note: It is assumed that given textures have same dimensions.
		void init(RenderTarget& texture1, RenderTarget& texture2);

		virtual void initDepth();
		void reset();

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNormal);
		virtual void fill(Drawer& drawer, Clearer clearer = clearNormal);

		virtual Resolution getResolution() const;
		const RenderTarget& getTexture(unsigned int position = 0) const;

		virtual void setRenderResolution() const;
		virtual void resize(ResolutionScale resolution);

		virtual std::string toString() const;

	private:
		RenderTarget* current;
		RenderTarget* first;
		RenderTarget* second;

		void swap();

		PingPongBuffer(const PingPongBuffer& other) = delete;
		PingPongBuffer& operator= (const PingPongBuffer& other) = delete;

	};

}

#endif
