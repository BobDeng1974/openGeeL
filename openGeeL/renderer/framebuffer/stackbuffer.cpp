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


	void StackBuffer::push(RenderTexture& texture) {
		stackBuffer.push(&texture);
	}

	void StackBuffer::pop() {
		stackBuffer.pop();

		//Restore render settings of previous element of stack (If it exists)
		//This is necessary since drawcall of current element may has its own settings
		if (!stackBuffer.empty()) {
			RenderTexture* previous = stackBuffer.top();
			bind();
			previous->assignTo(*this, 0);
			previous->setRenderResolution();
		}
	}


	void StackBuffer::init() {
		glGenFramebuffers(1, &fbo.token);
		bind();
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		unbind();
	}

	void StackBuffer::initResolution(const Resolution& resolution) {
		this->resolution = resolution;

		bind();
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.getWidth(), resolution.getHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}
	

	void StackBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		RenderTexture* current = stackBuffer.top();
		
		bind();
		current->assignTo(*this, 0);
		current->setRenderResolution();
		clearer.clear();

		drawCall();
		if(stackBuffer.size() <= 1) unbind();

		pop();
	}

	void StackBuffer::fill(Drawer& drawer, Clearer clearer) {
		RenderTexture* current = stackBuffer.top();

		bind();
		current->assignTo(*this, 0);
		current->setRenderResolution();
		clearer.clear();

		drawer.draw();
		if (stackBuffer.size() <= 1) unbind();

		pop();
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