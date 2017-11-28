#define GLEW_STATIC
#include <glew.h>
#include <list>
#include <iostream>
#include "glwrapper/viewport.h"
#include "renderer/renderer.h"
#include "framebuffer.h"

using namespace std;

namespace geeL {

	

	void FrameBuffer::fill(Drawer& drawer, Clearer clearer) {
		fill([&drawer]() {
			drawer.draw();
		}, clearer);
	}

	unsigned int IFrameBuffer::activeFBO = 0;
	void FrameBuffer::bind() const {
		if (fbo.token != IFrameBuffer::activeFBO) {
			IFrameBuffer::activeFBO = fbo.token;
			glBindFramebuffer(GL_FRAMEBUFFER, fbo.token);
		}
	}

	void FrameBuffer::bind(unsigned int fbo) {
		if (fbo != IFrameBuffer::activeFBO) {
			IFrameBuffer::activeFBO = fbo;
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		}
	}

	void IFrameBuffer::unbind() {
		IFrameBuffer::activeFBO = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::referenceRBO(FrameBuffer& buffer) {
		referenceRBO(buffer.rbo);
	}

	void FrameBuffer::referenceRBO(const RenderBufferToken & rbo) {
		this->rbo = rbo;
		bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}

	void FrameBuffer::copyRBO(FrameBuffer& buffer) {
		if (rbo == buffer.rbo) return;

		const Resolution& res = getResolution();

		glStencilMask(0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer.getFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.token);
		glBlitFramebuffer(0, 0, res.getWidth(), res.getHeight(), 0, 0,
			res.getWidth(), res.getHeight(), GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::copyDepth(const FrameBuffer& buffer) const {
		if (rbo == buffer.rbo) return;

		const Resolution& res = getResolution();

		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer.getFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.token);
		glBlitFramebuffer(0, 0, res.getWidth(), res.getHeight(), 0, 0,
			res.getWidth(), res.getHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::copyStencil(const FrameBuffer & buffer) const {
		if (rbo == buffer.rbo) return;

		const Resolution& res = getResolution();

		glStencilMask(0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer.getFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.token);
		glBlitFramebuffer(0, 0, res.getWidth(), res.getHeight(), 0, 0,
			res.getWidth(), res.getHeight(), GL_STENCIL_BUFFER_BIT, GL_NEAREST);
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

	const Resolution& FrameBuffer::getResolution() const {
		return resolution;
	}

}