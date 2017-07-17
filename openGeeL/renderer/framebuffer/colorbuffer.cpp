#define GLEW_STATIC
#include <glew.h>
#include <array>
#include <iostream>
#include "../renderer.h"
#include "colorbuffer.h"

using namespace std;

namespace geeL {


	ColorBuffer::~ColorBuffer() {
		remove();

		for (auto it = buffers.begin(); it != buffers.end(); it++) {
			RenderTexture* texture = *it;
			texture->remove();

			delete texture;
		}
	}

	void ColorBuffer::init(Resolution resolution, std::vector<RenderTexture*>&& colorBuffers) {
		buffers = std::move(colorBuffers);

		this->resolution = resolution;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

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

	void ColorBuffer::init(Resolution resolution, ColorType colorType, FilterMode filterMode, WrapMode wrapMode) {
		//std::cout << resolution.toString() << "\n";
		this->resolution = resolution;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// Create color attachment textures
		RenderTexture* texture = new RenderTexture(resolution, colorType, wrapMode, filterMode);
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
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.getWidth(), resolution.getHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}

	void ColorBuffer::resize(ResolutionScale scale) {
		resolution.resize(scale);

		for (auto buffer = buffers.begin(); buffer != buffers.end(); buffer++)
			(*buffer)->resize(resolution);

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.getWidth(), resolution.getHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}


	void ColorBuffer::fill(std::function<void()> drawCall) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, resolution.getWidth(), resolution.getHeight());
		clear();

		drawCall();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ColorBuffer::fill(Drawer& drawer) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, resolution.getWidth(), resolution.getHeight());
		clear();

		drawer.draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	const RenderTexture& ColorBuffer::getTexture(unsigned int position) const {
		if (position >= buffers.size())
			throw "Committed position out of bounds";

		return *buffers[position];
	}

	std::string ColorBuffer::toString() const {
		std::string s = "Color buffer " + std::to_string(fbo) + "\n";

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