#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "../renderer.h"
#include "pingpong.h"

using namespace std;

namespace geeL {

	PingPongBuffer::~PingPongBuffer() {
		remove();

		if (!external) {
			if (first != nullptr) delete first;
			if (second != nullptr) delete second;
		}
	}

	void PingPongBuffer::init(RenderTexture& texture1, RenderTexture& texture2) {
		external = true;
	
		glGenFramebuffers(1, &fbo);
		bind();

		first = &texture1;
		second = &texture2;

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, first->getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, second->getID(), 0);

		reset();

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Pingpong buffer is not complete!" << endl;

		unbind();
	}


	void PingPongBuffer::init(Resolution resolution, ColorType colorType, FilterMode filterMode, WrapMode wrapMode) {
		external = false;

		glGenFramebuffers(1, &fbo);
		bind();

		first = new RenderTexture(resolution, colorType, wrapMode, filterMode);
		second = new RenderTexture(resolution, colorType, wrapMode, filterMode);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, first->getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, second->getID(), 0);

		reset();

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Pingpong buffer is not complete!" << endl;

		unbind();
	}

	void PingPongBuffer::fill(std::function<void()> drawCall) {
		unsigned int id = (current == first) ? 0 : 1;

		bind();
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + id);
		current->setRenderResolution();
		clear();

		drawCall();
		unbind();

		swap();
	}

	void PingPongBuffer::fill(Drawer & drawer) {
		unsigned int id = (current == first) ? 0 : 1;

		bind();
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + id);
		current->setRenderResolution();
		clear();

		drawer.draw();
		unbind();

		swap();
	}

	const Resolution& PingPongBuffer::getResolution() const {
		return current->getResolution();
	}

	const RenderTexture & PingPongBuffer::getTexture(unsigned int position) const {
		if (position == 0)
			return *first;
		else
			return *second;
	}

	void PingPongBuffer::resetSize() const {
		current->setRenderResolution();
	}

	void PingPongBuffer::resize(ResolutionScale resolution) {
		std::cout << "Pingpong buffer is not resizable\n";
	}

	void PingPongBuffer::swap() {
		if (current == first)
			current = second;
		else
			current = first;
	}

	void PingPongBuffer::initDepth() {
		if (!initialized()) {
			std::cout << "Color buffer needs to be initialized first before initializing depth\n";
			return;
		}

		bind();
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, getResolution().getWidth(), getResolution().getHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}

	void PingPongBuffer::reset() {
		current = first;
	}

	std::string PingPongBuffer::toString() const {
		return "Ping pong buffer";
	}

}