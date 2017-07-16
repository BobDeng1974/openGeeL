#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "../renderer.h"
#include "pingpong.h"

using namespace std;

namespace geeL {

	void PingPongBuffer::init(Resolution resolution, ColorType colorType, FilterMode filterMode, WrapMode wrapMode) {
		this->resolution = resolution;

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		RenderTexture* texture1 = new RenderTexture(resolution, colorType, wrapMode, filterMode);
		RenderTexture* texture2 = new RenderTexture(resolution, colorType, wrapMode, filterMode);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture1->getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture2->getID(), 0);

		buffers.push_back(texture1);
		buffers.push_back(texture2);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PingPongBuffer::fill(std::function<void()> drawCall) {
		unsigned int id = (current == buffers[0]) ? 0 : 1;

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + id);
		current->setRenderResolution();
		clear();

		drawCall();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		swap();
	}

	void PingPongBuffer::fill(Drawer & drawer) {
		unsigned int id = (current == buffers[0]) ? 0 : 1;

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + id);
		current->setRenderResolution();
		clear();

		drawer.draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		swap();
	}

	void PingPongBuffer::resetSize() const {
		current->setRenderResolution();
	}

	void PingPongBuffer::resize(ResolutionScale resolution) {
		std::cout << "Pingpong buffer is not resizable\n";
	}

	void PingPongBuffer::swap() {
		if (current == buffers[0])
			current = buffers[1];
		else
			current = buffers[0];
	}

	void PingPongBuffer::reset() {
		current = buffers[0];
	}

}