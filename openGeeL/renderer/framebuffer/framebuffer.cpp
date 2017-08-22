﻿#define GLEW_STATIC
#include <glew.h>
#include <list>
#include <iostream>
#include "utility/viewport.h"
#include "renderer.h"
#include "framebuffer.h"


using namespace std;

namespace geeL {

	FrameBufferToken::~FrameBufferToken() {
		if (token != 0)
			glDeleteFramebuffers(1, &token);

		std::cout << "Delete " << token << "\n";
	}


	void FrameBuffer::fill(Drawer& drawer) {
		fill([&drawer]() {
			drawer.draw();
		});
	}

	static unsigned int currentFBO = 0;
	void FrameBuffer::bind() const {
		if (fbo.token != currentFBO) {
			currentFBO = fbo.token;
			glBindFramebuffer(GL_FRAMEBUFFER, fbo.token);
		}
	}

	void FrameBuffer::bind(unsigned int fbo) {
		if (fbo != currentFBO) {
			currentFBO = fbo;
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		}
	}

	void FrameBuffer::unbind() {
		currentFBO = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::copyDepth(const FrameBuffer& buffer) const {
		const Resolution& res = getResolution();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer.getFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.token);
		glBlitFramebuffer(0, 0, res.getWidth(), res.getHeight(), 0, 0,
			res.getWidth(), res.getHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::resetSize() const {
		Viewport::set(0, 0, getResolution().getWidth(), getResolution().getHeight());
	}

	void FrameBuffer::resetSize(Resolution resolution) {
		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());
	}

	unsigned int FrameBuffer::getFBO() const {
		return fbo.token;
	}

	void FrameBuffer::clear() const {
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	const Resolution& FrameBuffer::getResolution() const {
		return resolution;
	}

}