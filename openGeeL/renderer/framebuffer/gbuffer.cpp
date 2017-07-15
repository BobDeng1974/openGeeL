#define GLEW_STATIC
#include <glew.h>
#include <climits>
#include <iostream>
#include "../shader/rendershader.h"
#include "../renderer.h"
#include "gbuffer.h"

namespace geeL {

	GBuffer::GBuffer(unsigned int width, unsigned int height, GBufferContent content) : content(content) {
	
		init(width, height);
	}

	GBuffer::~GBuffer() {
		delete positionRough;
		delete normalMet;
		delete diffuse;
		delete emissivity;

		remove();
	}


	void GBuffer::init(int width, int height) {
		info.setDimension(width, height);

		glGenFramebuffers(1, &info.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);

		//Create attachements for all color buffers
		initTextures(width, height);

		// Create a renderbuffer object for depth and stencil attachment
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << "\n";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GBuffer::fill(std::function<void()> drawCall) {
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);
		glViewport(0, 0, info.currWidth, info.currHeight);
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawCall();

		float data[4];
		glReadPixels(info.currWidth / 2, info.currHeight / 2, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.CTdepth = -data[2];

		int xOffset = info.currWidth / 3;
		int yOffset = info.currHeight / 3;
		float mini = 0.f;
		int maxDistance = 100;

		glReadPixels(xOffset, yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.TRdepth = (-data[2] < mini) ? maxDistance : -data[2];

		glReadPixels(info.currWidth - xOffset, yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.TLdepth = (-data[2] < mini) ? maxDistance : -data[2];

		glReadPixels(xOffset, info.currHeight - yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.BRdepth = (-data[2] < mini) ? maxDistance : -data[2];

		glReadPixels(info.currWidth - xOffset, info.currHeight - yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.BLdepth = (-data[2] < mini) ? maxDistance : -data[2];

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	float GBuffer::getDepth() const {
		return depthPos;
	}

	const RenderTexture& GBuffer::getDiffuse() const {
		return *diffuse;
	}

	const RenderTexture& GBuffer::getPositionRoughness() const {
		return *positionRough;
	}

	const RenderTexture& GBuffer::getNormalMetallic() const {
		return *normalMet;
	}

	const RenderTexture& GBuffer::getEmissivity() const {
		return *emissivity;
	}

	std::string GBuffer::getFragmentPath() const {
		switch (content) {
			case GBufferContent::DefaultEmissive:
				return "renderer/shaders/gbufferEmi.frag";
			default:
				return "renderer/shaders/gbuffer.frag";
		}
	}

	std::string GBuffer::toString() const {
		return "GBuffer " + std::to_string(info.fbo) + "\n"
			+ "--Diffuse " + std::to_string(diffuse->getID()) + "\n"
			+ "--PositionRoughness " + std::to_string(positionRough->getID()) + "\n"
			+ "--NormalMetallic " + std::to_string(normalMet->getID()) + "\n";
	}

	void GBuffer::initTextures(int width, int height) {
		positionRough = new RenderTexture(width, height, ColorType::RGBA16);
		normalMet = new RenderTexture(width, height, ColorType::RGBA16);
		diffuse = new RenderTexture(width, height, ColorType::RGBA);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionRough->getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalMet->getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, diffuse->getID(), 0);
		
		switch (content) {
			case GBufferContent::DefaultEmissive: {
				emissivity = new RenderTexture(width, height, ColorType::RGB);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, emissivity->getID(), 0);

				unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glDrawBuffers(4, attachments);
				break;
			}
			default: {
				emissivity = new RenderTexture();

				unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
				glDrawBuffers(3, attachments);
				break;
			}
		}
	}

}