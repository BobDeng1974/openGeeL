#define GLEW_STATIC
#include <glew.h>
#include <array>
#include <iostream>
#include "glwrapper/viewport.h"
#include "renderer/glstructures.h"
#include "texturing/rendertexture.h"
#include "colorbuffer.h"

using namespace std;

namespace geeL {


	ColorBuffer::~ColorBuffer() {
		for (auto it = buffers.begin(); it != buffers.end(); it++) {
			//Only remove texture if it was created by this color buffer
			if (it->first) {
				RenderTarget* texture = it->second;
				delete texture;
			}
		}
	}

	void ColorBuffer::init(Resolution resolution, std::vector<RenderTarget*>& colorBuffers) {
		for (auto it = colorBuffers.begin(); it != colorBuffers.end(); it++) {
			RenderTarget* texture = *it;
			buffers.push_back(pair<bool, RenderTarget*>(false, texture));
		}

		this->resolution = resolution;

		glGenFramebuffers(1, &fbo.token);
		bind();

		int amount = min(3, (int)buffers.size());
		if (colorBuffers.size() > 3)
			std::cout << "Only first 3 textures are used in color buffer\n";

		// Create color attachment textures
		for (int i = 0; i < amount; i++) {
			RenderTarget& tex = *buffers[i].second;
			tex.assignTo(*this, i);
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

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		unbind();
	}

	void ColorBuffer::init(Resolution resolution, RenderTarget& texture) {
		this->resolution = resolution;

		glGenFramebuffers(1, &fbo.token);
		bind();

		texture.assignTo(*this, 0);
		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);

		buffers.push_back(pair<bool, RenderTarget*>(false, &texture));

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		unbind();
	}

	void ColorBuffer::init(Resolution resolution, ColorType colorType, FilterMode filterMode, WrapMode wrapMode) {
		this->resolution = resolution;

		glGenFramebuffers(1, &fbo.token);
		bind();

		// Create color attachment textures
		RenderTexture* texture = new RenderTexture(resolution, colorType, wrapMode, filterMode);
		texture->assignTo(*this, 0);
		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);
		buffers.push_back(pair<bool, RenderTexture*>(true, texture));


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		unbind();
	}

	void ColorBuffer::initDepth() {
		if (!initialized()) {
			std::cout << "Color buffer needs to be initialized first before initializing depth\n";
			return;
		}

		bind();
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.getWidth(), resolution.getHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}

	Resolution ColorBuffer::getResolution() const {
		return resolution;
	}


	void ColorBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		bind();
		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());
		clearer.clear();

		drawCall();
		unbind();
	}

	void ColorBuffer::fill(Drawer& drawer, Clearer clearer) {
		bind();
		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());
		clearer.clear();

		drawer.draw();
		unbind();
	}

	const RenderTarget& ColorBuffer::getTexture(unsigned int position) const {
		if (position >= buffers.size())
			throw "Committed position out of bounds";

		return *buffers[position].second;
	}

	std::string ColorBuffer::toString() const {
		std::string s = "Color buffer " + std::to_string(fbo.token) + "\n";
		return s;
	}

}