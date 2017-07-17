#ifndef PINGPONGBUFFER_H
#define PINGPONGBUFFER_H

#include "framebuffer.h"

namespace geeL {

	//Buffer that uses different textures to draw images in alternating order
	class PingPongBuffer : public FrameBuffer {

	public:
		PingPongBuffer() : first(nullptr), second(nullptr) {}
		~PingPongBuffer();

		//Init buffer with external textures which will be filled by subsequent drawcalls
		//Note: Memory of render textures won't be managed by this color buffer
		//Note: It is assumed that given textures have same dimensions.
		void init(RenderTexture& texture1, RenderTexture& texture2);

		//Init buffer and create own render textures with given parameters
		//Note: Memory of these render texture will be managed by this color buffer
		void init(Resolution resolution, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::None, WrapMode wrapMode = WrapMode::ClampEdge);

		virtual void initDepth();
		void reset();

		virtual void fill(std::function<void()> drawCall);
		virtual void fill(Drawer& drawer);

		virtual const Resolution& getResolution() const;
		const RenderTexture& getTexture(unsigned int position = 0) const;

		virtual void resetSize() const;
		virtual void resize(ResolutionScale resolution);

		virtual std::string toString() const;

	private:
		RenderTexture* current;
		RenderTexture* first;
		RenderTexture* second;
		unsigned int rbo;
		bool external;

		void swap();

		PingPongBuffer(const PingPongBuffer& other) = delete;
		PingPongBuffer& operator= (const PingPongBuffer& other) = delete;

	};

}

#endif
