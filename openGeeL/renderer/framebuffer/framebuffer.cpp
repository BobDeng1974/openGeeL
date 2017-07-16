#define GLEW_STATIC
#include <glew.h>
#include <array>
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
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);
	}

	void FrameBuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::copyDepth(const FrameBuffer& buffer) const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer.getFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, info.fbo);
		glBlitFramebuffer(0, 0, info.currWidth, info.currHeight, 0, 0,
			info.currWidth, info.currHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::resetSize() const {
		glViewport(0, 0, info.currWidth, info.currHeight);
	}

	void FrameBuffer::resetSize(int width, int height) {
		glViewport(0, 0, width, height);
	}

	void FrameBuffer::remove() {
		glDeleteFramebuffers(1, &info.fbo);
	}

	unsigned int FrameBuffer::getFBO() const {
		return info.fbo;
	}

	unsigned int FrameBuffer::getWidth() const {
		return info.currWidth;
	}

	unsigned int FrameBuffer::getHeight() const {
		return info.currHeight;
	}



	ColorBuffer::~ColorBuffer() {
		remove();

		for (auto it = buffers.begin(); it != buffers.end(); it++) {
			RenderTexture* texture = *it;
			texture->remove();

			delete texture;
		}
	}

	void ColorBuffer::init(unsigned int width, unsigned int height, std::vector<RenderTexture*>&& colorBuffers) {
		buffers = std::move(colorBuffers);

		info.setDimension(width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glGenFramebuffers(1, &info.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);

		int amount = min(3, (int)buffers.size());
		if (colorBuffers.size() > 3)
			std::cout << "Only first 3 textures are used in color buffer\n";

		// Create color attachment textures
		for (int i = 0; i < amount; i++)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, buffers[i]->getID(), 0);

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

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ColorBuffer::init(unsigned int width, unsigned int height,
		ColorType colorType, FilterMode filterMode, WrapMode wrapMode) {
		
		info.setDimension(width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glGenFramebuffers(1, &info.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);

		// Create color attachment textures
		RenderTexture* texture = new RenderTexture(width, height, colorType, wrapMode, filterMode);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getID(), 0);
		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);
		buffers.push_back(texture);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ColorBuffer::initDepth() {
		if (!initialized()) {
			std::cout << "Color buffer needs to be initialized first before initializing depth\n";
			return;
		}

		bind();
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, info.currWidth, info.currHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}

	void ColorBuffer::resize(ResolutionScale resolution) {
		info.currWidth = unsigned int(info.baseWidth * resolution);
		info.currHeight = unsigned int(info.baseHeight * resolution);

		//TODO: make this more orderly
		glBindTexture(GL_TEXTURE_2D, buffers[0]->getID());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, info.currWidth, info.currHeight, 0, GL_RGBA, GL_FLOAT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, info.currWidth, info.currHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}


	void ColorBuffer::fill(std::function<void()> drawCall) {
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);
		glViewport(0, 0, info.currWidth, info.currHeight);
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawCall();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ColorBuffer::fill(Drawer& drawer) {
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);
		glViewport(0, 0, info.currWidth, info.currHeight);
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawer.draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	const RenderTexture& ColorBuffer::getTexture(unsigned int position) const {
		if (position >= buffers.size())
			throw "Committed position out of bounds";

		return *buffers[position];
	}

	std::string ColorBuffer::toString() const {
		std::string s =  "Color buffer " + std::to_string(info.fbo) + "\n";

		unsigned int counter = 0;
		for (auto it = buffers.begin(); it != buffers.end(); it++) {
			RenderTexture* texture = *it;

			std::string line = "--Texture " + std::to_string(counter) + ": " + std::to_string(texture->getID()) + "\n";
			s += line;

			counter++;
		}

		return s;
	}

	
}