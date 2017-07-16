#ifndef STACKBUFFER_H
#define STACKBUFFER_H

#include "framebuffer.h"

namespace geeL {

	//Stack buffer that uses different textures to draw images in alternating order
	class PingPongBuffer : public ColorBuffer {

	public:
		void reset();

		void init(Resolution resolution, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::None, WrapMode wrapMode = WrapMode::ClampEdge);

		virtual void fill(std::function<void()> drawCall);
		virtual void fill(Drawer& drawer);

		virtual void resetSize() const;
		virtual void resize(ResolutionScale resolution);

	private:
		RenderTexture* current;

		void swap();

	};

}

#endif
