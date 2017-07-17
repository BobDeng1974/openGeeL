#ifndef STACKBUFFER_H
#define STACKBUFFER_H

#include <stack>
#include "framebuffer.h"

namespace geeL {

	//Buffer that operates as a stack and whose drawcalls will be executed in stack order
	class StackBuffer : public FrameBuffer {

	public:
		StackBuffer();
		~StackBuffer();

		//Adds a new render texture to the stack. The current texture (Top of stack)
		//will always be used for the next draw/fill call and then removed from the stack.
		//Note: Memory of render texture will not be managed by this buffer
		void push(RenderTexture* texture);

		virtual void fill(std::function<void()> drawCall);
		virtual void fill(Drawer& drawer);

		virtual const Resolution& getResolution() const;
		virtual void resetSize() const;
		virtual void resize(ResolutionScale resolution);

		virtual std::string toString() const;

	private:
		std::stack<RenderTexture*> stackBuffer;

		void init();

		StackBuffer(const StackBuffer& other) = delete;
		StackBuffer& operator= (const StackBuffer& other) = delete;

	};

}

#endif
