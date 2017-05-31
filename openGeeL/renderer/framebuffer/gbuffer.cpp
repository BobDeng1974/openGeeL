#define GLEW_STATIC
#include <glew.h>
#include <climits>
#include <iostream>
#include "../shader/shader.h"
#include "../renderer.h"
#include "gbuffer.h"

namespace geeL {

	GBuffer::GBuffer() {}

	GBuffer::GBuffer(const GBuffer& buffer) : FrameBuffer(buffer), screenInfo(buffer.screenInfo), 
		diffuseSpec(buffer.diffuseSpec), positionDepth(buffer.positionDepth), 
		normalMet(buffer.normalMet), depthPos(buffer.depthPos) {}

	GBuffer::~GBuffer() {
		positionDepth.remove();
		normalMet.remove();
		diffuseSpec.remove();
	}


	void GBuffer::init(int width, int height) {
		info.width = width;
		info.height = height;

		glGenFramebuffers(1, &info.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);

		//Create attachements for all color buffers
		positionDepth = RenderTexture(width, height, ColorType::RGBA16);
		normalMet = RenderTexture(width, height, ColorType::RGBA16);
		diffuseSpec = RenderTexture(width, height, ColorType::RGBA);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionDepth.getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalMet.getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, diffuseSpec.getID(), 0);
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

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
		glViewport(0, 0, info.width, info.height);
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawCall();

		float data[4];
		glReadPixels(info.width / 2, info.height / 2, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.CTdepth = -data[2];

		int xOffset = info.width / 3;
		int yOffset = info.height / 3;
		float mini = 0.f;
		int maxDistance = 100;

		glReadPixels(xOffset, yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.TRdepth = (-data[2] < mini) ? maxDistance : -data[2];

		glReadPixels(info.width - xOffset, yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.TLdepth = (-data[2] < mini) ? maxDistance : -data[2];

		glReadPixels(xOffset, info.height - yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.BRdepth = (-data[2] < mini) ? maxDistance : -data[2];

		glReadPixels(info.width - xOffset, info.height - yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.BLdepth = (-data[2] < mini) ? maxDistance : -data[2];

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	float GBuffer::getDepth() const {
		return depthPos;
	}

	const RenderTexture& GBuffer::getDiffuseSpecular() const {
		return diffuseSpec;
	}

	const RenderTexture& GBuffer::getPositionDepth() const {
		return positionDepth;
	}

	const RenderTexture& GBuffer::getNormalMetallic() const {
		return normalMet;
	}

	std::string GBuffer::toString() const {
		std::string s = "GBuffer " + std::to_string(info.fbo) + "\n"
			+ "--DiffuseSpec " + std::to_string(diffuseSpec.getID()) + "\n"
			+ "--PositionDepth " + std::to_string(positionDepth.getID()) + "\n"
			+ "--NormalMetallic " + std::to_string(normalMet.getID()) + "\n";

		return s;
	}

}