#define GLEW_STATIC
#include <glew.h>
#include <list>
#include <iostream>
#include "glwrapper/viewport.h"
#include "renderer/glstructures.h"
#include "framebuffer.h"

using namespace std;

namespace geeL {

	void IFrameBuffer::fill(Drawer& drawer, Clearer clearer) {
		fill([&drawer]() {
			drawer.draw();
		}, clearer);
	}

	unsigned int IFrameBuffer::activeFBO = 0;
	void IFrameBuffer::bind() const {
		unsigned int fbo = getFBO();

		if (fbo != IFrameBuffer::activeFBO) {
			IFrameBuffer::activeFBO = fbo;
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		}
	}

	void IFrameBuffer::bind(unsigned int fbo) {
		if (fbo != IFrameBuffer::activeFBO) {
			IFrameBuffer::activeFBO = fbo;
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		}
	}

	void IFrameBuffer::unbind() {
		IFrameBuffer::activeFBO = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void IFrameBuffer::applyRenderResolution() const {
		Resolution res(getResolution());
		Viewport::set(0, 0, res.getWidth(), res.getHeight());
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

		Resolution res(getResolution());

		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer.getFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.token);
		glBlitFramebuffer(0, 0, res.getWidth(), res.getHeight(), 0, 0,
			res.getWidth(), res.getHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::copyStencil(const FrameBuffer & buffer) const {
		if (rbo == buffer.rbo) return;

		Resolution res(getResolution());

		glStencilMask(0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer.getFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.token);
		glBlitFramebuffer(0, 0, res.getWidth(), res.getHeight(), 0, 0,
			res.getWidth(), res.getHeight(), GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	}

	unsigned int FrameBuffer::getFBO() const {
		return fbo.token;
	}

}