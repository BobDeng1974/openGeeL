#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "texturing/texture.h"
#include "shadowbuffer.h"

namespace geeL {

	DepthFrameBuffer::DepthFrameBuffer() 
		: clearer(ClearingMethod::Depth)
		, current(nullptr) {
	
		glGenFramebuffers(1, &fbo.token);

		bind();
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		unbind();
	}


	void DepthFrameBuffer::add(Texture& texture) {
		current = &texture;

		bind();

		assert(texture.getColorType() == ColorType::Depth || texture.getColorType() == ColorType::Depth32);
		texture.assignDepth();

		unbind();
	}


	void DepthFrameBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		assert(current != nullptr);

		bind();
		current->applyRenderResolution();
		this->clearer.clear();

		drawCall();

		unbind();

	}

	Resolution DepthFrameBuffer::getResolution() const {
		assert(current != nullptr);
		return current->getScreenResolution();
	}

	unsigned int DepthFrameBuffer::getFBO() const {
		return fbo.token;
	}

}