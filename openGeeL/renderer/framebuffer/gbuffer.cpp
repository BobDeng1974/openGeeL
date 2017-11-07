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

	GBuffer::GBuffer(Resolution resolution, GBufferContent content) 
		: content(content), emissivity(nullptr), occlusion(nullptr) {
		
		init(resolution);
	}

	GBuffer::~GBuffer() {
		delete positionRough;
		delete normalMet;
		delete diffuse;
		
		if (emissivity != nullptr) delete emissivity;
		if (occlusion != nullptr) delete occlusion;
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

	const RenderTexture* GBuffer::getEmissivity() const {
		return emissivity;
	}

	const RenderTexture* GBuffer::getOcclusion() const {
		return occlusion;
	}

	RenderTexture& GBuffer::requestOcclusion(const ResolutionScale& scale) {
		if(occlusion == nullptr)
			occlusion = new RenderTexture(Resolution(resolution, scale), ColorType::Single, WrapMode::ClampEdge, FilterMode::None);

		return *occlusion;
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
		
		positionRough->assignTo(*this, 0);
		normalMet->assignTo(*this, 1);
		diffuse->assignTo(*this, 2);
		
		switch (content) {
			case GBufferContent::DefaultEmissive: {
				emissivity = new RenderTexture(resolution, ColorType::RGB);
				occlusion = new RenderTexture(resolution, ColorType::Single, WrapMode::ClampEdge, FilterMode::None);

				emissivity->assignTo(*this, 3);
				occlusion->assignTo(*this, 4);

				unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, 
					GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };

				glDrawBuffers(5, attachments);
				break;
			}
			default: {
				unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
				glDrawBuffers(3, attachments);
				break;
			}
		}
	}

	ForwardBuffer::ForwardBuffer(GBuffer& gBuffer) : gBuffer(gBuffer) {
		this->resolution = gBuffer.getResolution();

		init();
	}

	void ForwardBuffer::init() {
		glGenFramebuffers(1, &fbo.token);
		bind();

		//Create attachements for all color buffers
		gBuffer.getPositionRoughness().assignToo(*this, 0);
		gBuffer.getNormalMetallic().assignToo(*this, 1);
		gBuffer.getDiffuse().assignToo(*this, 2);
		//TODO: add emissivity texture

		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);

		referenceRBO(gBuffer);

		unbind();
	}

	void ForwardBuffer::setColorTexture(RenderTexture& colorTexture) {
		bind();
		colorTexture.assignTo(*this, 3);

		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);
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