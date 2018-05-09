#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "renderer/glstructures.h"
#include "rendertarget.h"
#include "pingpong.h"

using namespace std;

namespace geeL {

	PingPongBuffer::PingPongBuffer() 
		: first(nullptr)
		, second(nullptr) {}


	void PingPongBuffer::init(RenderTarget& texture1, RenderTarget& texture2) {
		glGenFramebuffers(1, &fbo.token);
		bind();

		first = &texture1;
		second = &texture2;

		first->assignTo(*this, 0);
		second->assignTo(*this, 1);

		reset();

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Pingpong buffer is not complete!" << endl;

		unbind();
	}


	void PingPongBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		unsigned int id = (current == first) ? 0 : 1;

		bind();
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + id);
		current->applyRenderResolution();
		clearer.clear();

		drawCall();
		unbind();

		swap();
	}

	void PingPongBuffer::fill(Drawer & drawer, Clearer clearer) {
		unsigned int id = (current == first) ? 0 : 1;

		bind();
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + id);
		current->applyRenderResolution();
		clearer.clear();

		drawer.draw();
		unbind();

		swap();
	}

	Resolution PingPongBuffer::getResolution() const {
		return current->getRenderResolution();
	}

	const RenderTarget& PingPongBuffer::getTexture(unsigned int position) const {
		if (position == 0)
			return *first;
		else
			return *second;
	}

	void PingPongBuffer::applyRenderResolution() const {
		current->applyRenderResolution();
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