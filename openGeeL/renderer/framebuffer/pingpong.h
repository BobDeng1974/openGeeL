#ifndef PINGPONGBUFFER_H
#define PINGPONGBUFFER_H

#include "framebuffer.h"

namespace geeL {

	//Buffer that uses different textures to draw images in alternating order
	class PingPongBuffer : public ColorBuffer {

	public:
		PingPongBuffer() {}
		~PingPongBuffer();

		void init(Resolution resolution, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::None, WrapMode wrapMode = WrapMode::ClampEdge);

		void reset();

		virtual void fill(std::function<void()> drawCall);
		virtual void fill(Drawer& drawer);

		virtual const Resolution& getResolution() const;
		virtual void resetSize() const;
		virtual void resize(ResolutionScale resolution);

	private:
		RenderTexture* current;

		void swap();

		PingPongBuffer(const PingPongBuffer& other) = delete;
		PingPongBuffer& operator= (const PingPongBuffer& other) = delete;

	};

}

#endif
