#define GLEW_STATIC
#include <glew.h>
#include <array>
#include <list>
#include <iostream>
#include "../renderer.h"
#include "framebuffer.h"


using namespace std;

namespace geeL {

	FrameBuffer::FrameBuffer() {}

	FrameBuffer::~FrameBuffer() {
		remove();
	}


	void FrameBuffer::init(unsigned int width, unsigned int height, int colorBufferAmount,
		ColorType colorType, FilterMode filterMode, WrapMode wrapMode, bool useDepth) {
		
		int amount = min(3, colorBufferAmount);
		vector<ColorType> bufferTypes = vector<ColorType>(amount, colorType);
		init(width, height, amount, bufferTypes, filterMode, wrapMode, useDepth);
	}

	void FrameBuffer::init(unsigned int width, unsigned int height, int colorBufferAmount, vector<ColorType> bufferTypes,
		FilterMode filterMode, WrapMode wrapMode, bool useDepth) {

		if (colorBufferAmount != bufferTypes.size())
			throw "Given amount of buffer types doesn't match amount of given color buffers";

		info.width = width;
		info.height = height;

		glGenFramebuffers(1, &info.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);

		// Create color attachment textures
		int amount = min(3, colorBufferAmount);
		for (int i = 0; i < amount; i++) {
			unsigned int color = generateTexture(bufferTypes[i], filterMode, wrapMode);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color, 0);
			colorBuffers.push_back(color);
		}

		switch (amount) {
			case 1: {
				GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers(amount, attachments);
			}
					break;
			case 2: {
				GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
				glDrawBuffers(amount, attachments);
			}
					break;
			case 3: {
				GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
				glDrawBuffers(amount, attachments);
			}
			break;
		}

		if (useDepth) {
			// Create a renderbuffer object for depth and stencil attachment
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);
	}

	void FrameBuffer::bind(unsigned int fbo) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	void FrameBuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::remove(unsigned int fbo) {
		glDeleteFramebuffers(1, &fbo);
	}

	void FrameBuffer::remove() {
		glDeleteFramebuffers(1, &info.fbo);
	}

	void FrameBuffer::resetSize(int width, int height) {
		glViewport(0, 0, width, height);
	}

	void FrameBuffer::resize(int width, int height) {
		if (width > 0 && height > 0) {
			info.width = width;
			info.height = height;

			//TODO: make this more orderly
			glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, info.width, info.height, 0, GL_RGBA, GL_FLOAT, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, info.width, info.height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}

	void FrameBuffer::fill(Drawer& drawer, bool setFBO) const {
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);
		glViewport(0, 0, info.width, info.height);
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(setFBO)
			drawer.setParentFBO(info.fbo);
		
		drawer.draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::copyDepth(unsigned int targetFBO) const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, targetFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, info.fbo);
		glBlitFramebuffer(0, 0, info.width, info.height, 0, 0,
			info.width, info.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	unsigned int FrameBuffer::generateTexture(ColorType colorType, FilterMode filterMode, WrapMode wrapMode) {

		//Generate texture ID and load texture data 
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		Texture::initColorType(colorType, info.width, info.height, 0);
		Texture::initWrapMode(wrapMode);
		Texture::initFilterMode(filterMode);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}

	unsigned int FrameBuffer::getColorID(unsigned int position) const {
		if (position >= colorBuffers.size())
			throw "Committed postion out of bounds";

		return colorBuffers[position];
	}

	unsigned int FrameBuffer::getFBO() const {
		return info.fbo;
	}

	unsigned int FrameBuffer::getWidth() const {
		return info.width;
	}

	unsigned int FrameBuffer::getHeight() const {
		return info.height;
	}
}