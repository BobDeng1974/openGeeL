#define GLEW_STATIC
#include <glew.h>

#include <list>
#include <iostream>
#include "../renderer.h"
#include "framebuffer.h"


using namespace std;

namespace geeL {

	void FrameBuffer::fill(Drawer& drawer) {
		fill([&drawer]() {
			drawer.draw();
		});
	}


	void FrameBuffer::bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	void FrameBuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::copyDepth(const FrameBuffer& buffer) const {
		const Resolution& res = getResolution();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer.getFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glBlitFramebuffer(0, 0, res.getWidth(), res.getHeight(), 0, 0,
			res.getWidth(), res.getHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::resetSize() const {
		glViewport(0, 0, getResolution().getWidth(), getResolution().getHeight());
	}

	void FrameBuffer::resetSize(Resolution resolution) {
		glViewport(0, 0, resolution.getWidth(), resolution.getHeight());
	}

	void FrameBuffer::remove() {
		glDeleteFramebuffers(1, &fbo);
	}

	unsigned int FrameBuffer::getFBO() const {
		return fbo;
	}

	void FrameBuffer::clear() const {
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	const Resolution& FrameBuffer::getResolution() const {
		return resolution;
	}


	

	
}