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


	void FrameBuffer::init(int width, int height, int colorBufferAmount,
		ColorBufferType colorBufferType, unsigned int filterMode, bool useDepth) {
		
		int amount = min(3, colorBufferAmount);
		vector<ColorBufferType> bufferTypes = vector<ColorBufferType>(amount, colorBufferType);
		init(width, height, amount, bufferTypes, filterMode, useDepth);
	}

	void FrameBuffer::init(int width, int height, int colorBufferAmount, vector<ColorBufferType> bufferTypes,
		unsigned int filterMode, bool useDepth) {

		if (colorBufferAmount != bufferTypes.size())
			throw "Committed amount of buffer types doesn't match amount of committed color buffers";

		this->width = width;
		this->height = height;

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// Create color attachment textures
		int amount = min(3, colorBufferAmount);
		for (int i = 0; i < amount; i++) {
			unsigned int color = generateTexture(bufferTypes[i], filterMode);
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
			GLuint rbo;
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
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	void FrameBuffer::bind(unsigned int fbo) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	void FrameBuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::resetSize(int width, int height) {
		glViewport(0, 0, width, height);
	}

	void FrameBuffer::fill(Drawer& drawer, bool setFBO) const {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, width, height);
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(setFBO)
			drawer.setParentFBO(fbo);
		
		drawer.draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::copyDepth(unsigned int targetFBO) const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, targetFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glBlitFramebuffer(0, 0, width, height, 0, 0,
			width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	unsigned int FrameBuffer::generateTexture(ColorBufferType colorBufferType, unsigned int filterMode) {

		//Generate texture ID and load texture data 
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		switch (colorBufferType) {
			case Single:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, 0);
				break;
			case RGB:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, 0);
				break;
			case RGBA:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);
				break;
			case RGB16:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
				break;
			case RGBA16:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
				break;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}

	unsigned int FrameBuffer::getColorID(unsigned int position) const {
		if (position >= colorBuffers.size())
			throw "Committed postion out of bounds";

		return colorBuffers[position];
	}
}