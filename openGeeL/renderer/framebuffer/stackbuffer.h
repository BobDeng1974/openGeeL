#ifndef STACKBUFFER_H
#define STACKBUFFER_H

#include <stack>
#include "framebuffer.h"

namespace geeL {

	//Buffer that operates as a stack and whose drawcalls will be executed in stack order
	class StackBuffer : public DynamicBuffer {

	public:
		StackBuffer();

		//Adds a new render texture to the stack. The current texture (Top of stack)
		//will always be used for the next draw/fill call and then removed from the stack.
		//Note: Memory of render texture will not be managed by this buffer
		virtual void push(RenderTarget& target);
		virtual void pop();

		void initResolution(const Resolution& resolution);

		virtual void fill(std::function<void()> drawCall, Clearer clearer = clearNormal);
		virtual void fill(Drawer& drawer, Clearer clearer = clearNormal);

		//virtual const RenderTexture* const getTexture() const;
		virtual const Resolution& getResolution() const;
		virtual void resetSize() const;
		virtual void resize(ResolutionScale resolution);

		size_t size() const;
		virtual std::string toString() const;

	private:
		std::stack<RenderTarget*> stackBuffer;

		void init();

		StackBuffer(const StackBuffer& other) = delete;
		StackBuffer& operator= (const StackBuffer& other) = delete;

	};


	inline size_t StackBuffer::size() const {
		return stackBuffer.size();
	}

}

#endif
