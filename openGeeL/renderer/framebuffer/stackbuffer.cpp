#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "renderer/glstructures.h"
#include "bufferutil.h"
#include "rendertarget.h"
#include "stackbuffer.h"

using namespace std;

namespace geeL {

	StackBuffer::StackBuffer(unsigned int size) 
		: size(size) {
		
		init(size);
	}


	void StackBuffer::push(RenderTarget& target) {
		assert(target.getSize() == size);
		stackBuffer.push(&target);
	}

	void StackBuffer::pop() {
		stackBuffer.pop();

		//Restore render settings of previous element of stack (If it exists)
		//This is necessary since drawcall of current element may has its own settings
		if (!stackBuffer.empty()) {
			RenderTarget* previous = stackBuffer.top();
			bind();
			previous->assignTo(*this, 0);
			previous->setRenderResolution();
		}
	}


	void StackBuffer::init(unsigned int size) {
		glGenFramebuffers(1, &fbo.token);
		setSize(size);
	}

	void StackBuffer::initResolution(const Resolution& resolution) {
		bind();
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.getWidth(), resolution.getHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}

	void StackBuffer::setSize(unsigned int size) {
		if (size > 0 && size <= 5) {
			bind();
			BufferUtility::drawBuffers(size);
			this->size = size;
			unbind();
		}
	}
	

	void StackBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		RenderTarget* current = stackBuffer.top();
		
		bind();
		current->assignTo(*this, 0);
		current->setRenderResolution();
		clearer.clear();

		drawCall();
		if(stackBuffer.size() <= 1) unbind();

		pop();
	}

	void StackBuffer::fill(Drawer& drawer, Clearer clearer) {
		RenderTarget* current = stackBuffer.top();

		bind();
		current->assignTo(*this, 0);
		current->setRenderResolution();
		clearer.clear();

		drawer.draw();
		if (stackBuffer.size() <= 1) unbind();

		pop();
	}

	Resolution StackBuffer::getResolution() const {
		assert(!stackBuffer.empty());

		return stackBuffer.top()->getRenderResolution();
	}

	void StackBuffer::setRenderResolution() const {
		if(!stackBuffer.empty())
			stackBuffer.top()->setRenderResolution();
	}

	void StackBuffer::resize(ResolutionScale resolution) {}

	std::string StackBuffer::toString() const {
		return "Stack Buffer";
	}

}