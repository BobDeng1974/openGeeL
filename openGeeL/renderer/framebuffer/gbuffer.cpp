#define GLEW_STATIC
#include <glew.h>
#include <climits>
#include <iostream>
#include "shader/rendershader.h"
#include "utility/viewport.h"
#include "utility/glguards.h"
#include "renderer.h"
#include "gbuffer.h"

namespace geeL {

	GBuffer::GBuffer(Resolution resolution, GBufferContent content) : content(content) {
		init(resolution);
	}

	GBuffer::~GBuffer() {
		delete positionRough;
		delete normalMet;
		delete diffuse;
		delete emissivity;
	}


	void GBuffer::init(Resolution resolution) {
		this->resolution = resolution;

		glGenFramebuffers(1, &fbo.token);
		bind();

		//Create attachements for all color buffers
		initTextures(resolution);

		//Create a renderbuffer object for depth and stencil attachment
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolution.getWidth(), resolution.getHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		unbind();
	}

	void GBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		bind();
		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());
		clearer.clear();

		drawCall();

		float data[4];
		glReadPixels(resolution.getWidth() / 2, resolution.getHeight() / 2, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.CTdepth = -data[2];

		int xOffset = resolution.getWidth() / 3;
		int yOffset = resolution.getHeight() / 3;
		float mini = 0.f;
		int maxDistance = 100;

		glReadPixels(xOffset, yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.TRdepth = (-data[2] < mini) ? maxDistance : -data[2];

		glReadPixels(resolution.getWidth() - xOffset, yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.TLdepth = (-data[2] < mini) ? maxDistance : -data[2];

		glReadPixels(xOffset, resolution.getHeight() - yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.BRdepth = (-data[2] < mini) ? maxDistance : -data[2];

		glReadPixels(resolution.getWidth() - xOffset, resolution.getHeight() - yOffset, 1, 1, GL_RGBA, GL_FLOAT, data);
		screenInfo.BLdepth = (-data[2] < mini) ? maxDistance : -data[2];
		
		unbind();
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
		return "GBuffer " + std::to_string(fbo.token) + "\n"
			+ "--Diffuse " + std::to_string(diffuse->getID()) + "\n"
			+ "--PositionRoughness " + std::to_string(positionRough->getID()) + "\n"
			+ "--NormalMetallic " + std::to_string(normalMet->getID()) + "\n";
	}

	void GBuffer::initTextures(Resolution resolution) {
		positionRough = new RenderTexture(resolution, ColorType::RGBA16);
		normalMet = new RenderTexture(resolution, ColorType::RGBA16);
		diffuse = new RenderTexture(resolution, ColorType::RGBA);
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionRough->getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalMet->getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, diffuse->getID(), 0);
		
		switch (content) {
			case GBufferContent::DefaultEmissive: {
				emissivity = new RenderTexture(resolution, ColorType::RGB);
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

	ForwardBuffer::ForwardBuffer(GBuffer& gBuffer) : gBuffer(gBuffer) {
		this->resolution = gBuffer.getResolution();
	}

	void ForwardBuffer::init(RenderTexture& colorTexture) {
		glGenFramebuffers(1, &fbo.token);
		bind();

		//Create attachements for all color buffers
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer.getPositionRoughness().getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer.getNormalMetallic().getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer.getDiffuse().getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, colorTexture.getID(), 0);

		//TODO: add emissivity texture

		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);

		referenceRBO(gBuffer);

		unbind();
	}

	void ForwardBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		bind();
		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());

		BlendGuard blend(3);
		blend.blendAlpha();

		drawCall();

		unbind();
	}


	std::string ForwardBuffer::toString() const {
		return "FBuffer " + std::to_string(fbo.token) + "\n";
	}


}