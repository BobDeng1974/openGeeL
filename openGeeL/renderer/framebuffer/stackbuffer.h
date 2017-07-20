#ifndef STACKBUFFER_H
#define STACKBUFFER_H

#include <stack>
#include "framebuffer.h"

namespace geeL {

	//Buffer that operates as a stack and whose drawcalls will be executed in stack order
	class StackBuffer : public DynamicBuffer {

	public:
		StackBuffer();
		~StackBuffer();

		//Adds a new render texture to the stack. The current texture (Top of stack)
		//will always be used for the next draw/fill call and then removed from the stack.
		//Note: Memory of render texture will not be managed by this buffer
		void push(RenderTexture& texture);
		virtual void add(RenderTexture& texture);

		void initResolution(const Resolution& resolution);

		virtual void fill(std::function<void()> drawCall);
		virtual void fill(Drawer& drawer);

		virtual const RenderTexture* const getTexture() const;
		virtual const Resolution& getResolution() const;
		virtual void resetSize() const;
		virtual void resize(ResolutionScale resolution);

		virtual std::string toString() const;

	private:
		unsigned int rbo;
		std::stack<RenderTexture*> stackBuffer;

		void init();
		void pop();

		void bindTexture(const Texture& texture, unsigned int position = 0);

		StackBuffer(const StackBuffer& other) = delete;
		StackBuffer& operator= (const StackBuffer& other) = delete;

	};

}

#endif
