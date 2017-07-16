#ifndef STACKBUFFER_H
#define STACKBUFFER_H

#include "framebuffer.h"

namespace geeL {

	//Stack buffer that uses two frame buffers to draw images in alternating order
	class StackBufferPingPong : public ColorBuffer {

	public:
		StackBufferPingPong();
		~StackBufferPingPong();

		void init(Resolution resolution, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::None, WrapMode wrapMode = WrapMode::ClampEdge);
		
		//Reset stack order
		void reset();

		virtual void bind() const;
		virtual void initDepth();

		virtual void fill(std::function<void()> drawCall);
		virtual void fill(Drawer& drawer);

		virtual void resize(ResolutionScale resolution);
		virtual void copyDepth(const FrameBuffer& buffer) const;

		const RenderTexture& getTexture(unsigned int position = 0) const;

		virtual void resetSize() const;
		virtual unsigned int getWidth() const;
		virtual unsigned int getHeight() const;
		
		virtual std::string toString() const;

	private:
		ColorBuffer first, second;
		ColorBuffer* current;

		void swap();

		StackBufferPingPong(const ColorBuffer& other) = delete;
		StackBufferPingPong& operator= (const ColorBuffer& other) = delete;

	};


	//Stack buffer that uses different textures to draw images in alternating order
	class StackBuffer : public ColorBuffer {

	public:
		void reset();

		void init(Resolution resolution, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::None, WrapMode wrapMode = WrapMode::ClampEdge);

		virtual void fill(std::function<void()> drawCall);
		virtual void fill(Drawer& drawer);

	private:
		RenderTexture* current;

		void swap();


	};

}

#endif
