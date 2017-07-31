#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "renderer.h"
#include "stackbuffer.h"

using namespace std;

namespace geeL {

	StackBuffer::StackBuffer() {
		init();
	}

	StackBuffer::~StackBuffer() {
		remove();
	}


	void StackBuffer::push(RenderTexture& texture) {
		stackBuffer.push(&texture);
	}

	void StackBuffer::add(RenderTexture& texture) {
		push(texture);
	}

	void StackBuffer::pop() {
		stackBuffer.pop();

		//Restore render settings of previous element of stack (If it exists)
		//This is necessary since drawcall of current element may has its own settings
		if (!stackBuffer.empty()) {
			RenderTexture* previous = stackBuffer.top();
			bind();
			bindTexture(*previous);
			previous->setRenderResolution();
			glClear(GL_DEPTH_BUFFER_BIT);
		}
	}


	void StackBuffer::init() {
		glGenFramebuffers(1, &fbo);
		bind();
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		unbind();
	}

	void StackBuffer::initResolution(const Resolution& resolution) {
		this->resolution = resolution;

		bind();
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.getWidth(), resolution.getHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}
	

	void StackBuffer::fill(std::function<void()> drawCall) {
		RenderTexture* current = stackBuffer.top();
		
		bind();
		bindTexture(*current);
		current->setRenderResolution();
		clear();

		drawCall();
		if(stackBuffer.size() <= 1) unbind();

		pop();
	}

	void StackBuffer::fill(Drawer& drawer) {
		RenderTexture* current = stackBuffer.top();

		bind();
		bindTexture(*current);
		current->setRenderResolution();
		clear();

		drawer.draw();
		if (stackBuffer.size() <= 1) unbind();

		pop();
	}


	void StackBuffer::bindTexture(const Texture& texture, unsigned int position) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + position, GL_TEXTURE_2D, texture.getID(), 0);
	}

	const RenderTexture* const StackBuffer::getTexture() const {
		if (!stackBuffer.empty())
			return stackBuffer.top();

		return nullptr;
	}

	const Resolution& StackBuffer::getResolution() const {
		if (stackBuffer.empty())
			return resolution;

		return stackBuffer.top()->getResolution();
	}

	void StackBuffer::resetSize() const {
		if(!stackBuffer.empty())
			stackBuffer.top()->setRenderResolution();
	}

	void StackBuffer::resize(ResolutionScale resolution) {}

	std::string StackBuffer::toString() const {
		return "Stack Buffer";
	}

}