#ifndef STACKBUFFER_H
#define STACKBUFFER_H

#include "framebuffer.h"

namespace geeL {

	class StackBuffer : public ColorBuffer {

	public:
		StackBuffer();
		~StackBuffer();

		void init(unsigned int width, unsigned int height, ColorType colorType = ColorType::RGBA16,
			FilterMode filterMode = FilterMode::None, WrapMode wrapMode = WrapMode::ClampEdge);
		
		//Reset stack order
		void reset();

		virtual void bind() const;
		virtual void initDepth();

		virtual void fill(std::function<void()> drawCall);
		virtual void fill(Drawer& drawer);

		virtual void resize(Resolution resolution);
		virtual void copyDepth(const FrameBuffer& buffer) const;

		const RenderTexture& getTexture(unsigned int position = 0) const;

		virtual void resetSize() const;
		virtual unsigned int getWidth() const;
		virtual unsigned int getHeight() const;
		
		virtual std::string toString() const;

	private:
		bool toggle;
		ColorBuffer first, second;
		ColorBuffer* current;

		void swap();

		StackBuffer(const ColorBuffer& other) = delete;
		StackBuffer& operator= (const ColorBuffer& other) = delete;

	};

}

#endif
