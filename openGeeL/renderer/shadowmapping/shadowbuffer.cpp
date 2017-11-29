#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "texturing/texture.h"
#include "shadowbuffer.h"

namespace geeL {

	DepthFrameBuffer::DepthFrameBuffer(const Resolution& defaultResolution) 
		: resolution(defaultResolution)
		, clearer(ClearingMethod::Depth)
		, current(nullptr) {
	
		glGenFramebuffers(1, &fbo.token);

		bind();
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		unbind();
	}


	void DepthFrameBuffer::add(Texture& texture) {
		current = &texture;
		currResolution = current->getScreenResolution();

		bind();

		//TODO: make this less shitty later on
		assert(texture.getColorType() == ColorType::Depth || texture.getColorType() == ColorType::Depth32);
		switch (texture.getTextureType()) {
			case TextureType::Texture2D:
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.getID(), 0);
				break;
			case TextureType::TextureCube:
				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture.getID(), 0);
				break;
		}

		unbind();
	}


	void DepthFrameBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		bind();
		resetSize();
		this->clearer.clear();

		drawCall();

		unbind();

	}

	const Resolution& DepthFrameBuffer::getResolution() const {
		if (current != nullptr)
			return currResolution;

		return resolution;
	}

	unsigned int DepthFrameBuffer::getFBO() const {
		return fbo.token;
	}

}