#ifndef PINGPONGBUFFER_H
#define PINGPONGBUFFER_H

#include "framebuffer.h"

namespace geeL {

	class RenderTarget;


	//Buffer that uses different textures to draw images in alternating order
	class PingPongBuffer : public FrameBuffer {

	public:
		PingPongBuffer() : first(nullptr), second(nullptr) {}
		virtual ~PingPongBuffer();

		//Init buffer with external textures which will be filled by subsequent drawcalls
		//Note: Memory of render textures won't be managed by this color buffer
		//Note: It is assumed that given textures have same dimensions.
		void init(RenderTarget& texture1, RenderTarget& texture2);

		//Init buffer and create own render textures with given parameters
		//Note: Memory of these render texture will be managed by this color buffer
		void init(Resolution resolution, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::None, WrapMode wrapMode = WrapMode::ClampEdge);

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
		bool external;

		void swap();

		PingPongBuffer(const PingPongBuffer& other) = delete;
		PingPongBuffer& operator= (const PingPongBuffer& other) = delete;

	};

}

#endif
