#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "../renderer.h"
#include "stackbuffer.h"

using namespace std;

namespace geeL {

	StackBuffer::StackBuffer() {
		init();
	}

	StackBuffer::~StackBuffer() {
		remove();
	}

	void StackBuffer::push(RenderTexture* texture) {
		stackBuffer.push(texture);
	}

	void StackBuffer::init() {

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void StackBuffer::fill(std::function<void()> drawCall) {
		RenderTexture* current = stackBuffer.top();

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, current->getID(), 0);
		current->setRenderResolution();
		clear();

		drawCall();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		stackBuffer.pop();
	}

	void StackBuffer::fill(Drawer& drawer) {
		RenderTexture* current = stackBuffer.top();

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, current->getID(), 0);
		current->setRenderResolution();
		clear();

		drawer.draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		stackBuffer.pop();
	}


	const Resolution& StackBuffer::getResolution() const {
		if (stackBuffer.empty())
			throw "Can't provide resolution since stack buffer is empty\n";

		stackBuffer.top()->getResolution();
	}

	void StackBuffer::resetSize() const {
		if(!stackBuffer.empty())
			stackBuffer.top()->setRenderResolution();
	}

	void StackBuffer::resize(ResolutionScale resolution) {
		//TODO: implement this
	}

	std::string StackBuffer::toString() const {
		return "Stack Buffer";
	}

}