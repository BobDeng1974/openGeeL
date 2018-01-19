#define GLEW_STATIC
#include <glew.h>
#include <climits>
#include <iostream>
#include "glwrapper/viewport.h"
#include "glwrapper/glguards.h"
#include "shader/rendershader.h"
#include "renderer/glstructures.h"
#include "appglobals.h"
#include "bufferutil.h"

#include "gbuffer.h"


namespace geeL {

	GBuffer::GBuffer(Resolution resolution) 
		: resolution(resolution)
		, occEmiRoughMet(nullptr) {
		
		init(resolution);
	}

	GBuffer::~GBuffer() {
		delete position;
		delete normal;
		delete diffuse;
		delete occEmiRoughMet;
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
		
		unbind();
	}

	Resolution GBuffer::getResolution() const {
		return resolution;
	}

	const RenderTexture& GBuffer::getDiffuse() const {
		return *diffuse;
	}

	const RenderTexture& GBuffer::getPosition() const {
		return *position;
	}

	const RenderTexture& GBuffer::getNormal() const {
		return *normal;
	}

	const RenderTexture& GBuffer::getProperties() const {
		return *occEmiRoughMet;
	}

	RenderTexture& GBuffer::getOcclusion() const {
		return *occEmiRoughMet;
	}

	std::string GBuffer::getFragmentPath() const {
		return "shaders/gbufferEmi.frag";
	}

	std::string GBuffer::toString() const {
		return "GBuffer " + std::to_string(fbo.token) + "\n";
	}

	void GBuffer::initTextures(Resolution resolution) {
		position = new RenderTexture(resolution, ColorType::RGB16);
		normal = new RenderTexture(resolution, ColorType::RGBA16);
		diffuse = new RenderTexture(resolution, ColorType::RGBA);
		occEmiRoughMet = new RenderTexture(resolution, ColorType::RGBA);
		
		position->assignTo(*this, 0);
		normal->assignTo(*this, 1);
		diffuse->assignTo(*this, 2);
		occEmiRoughMet->assignTo(*this, 3);
		
		unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);
	}

	ForwardBuffer::ForwardBuffer(GBuffer& gBuffer) 
		: gBuffer(gBuffer)
		, target(nullptr) {

		init();
	}

	void ForwardBuffer::init() {
		glGenFramebuffers(1, &fbo.token);
		bind();

		//Create attachements for all color buffers
		gBuffer.getPosition().assignToo(*this, 0);
		gBuffer.getNormal().assignToo(*this, 1);
		gBuffer.getDiffuse().assignToo(*this, 2);
		gBuffer.getProperties().assignToo(*this, 3);

#if DIFFUSE_SPECULAR_SEPARATION
		BufferUtility::drawBuffers(6);
#else
		BufferUtility::drawBuffers(5);
#endif

		referenceRBO(gBuffer);

		unbind();
	}

	void ForwardBuffer::setTarget(RenderTarget& colorTexture) {
		bind();
		colorTexture.assignTo(*this, 4);
		target = &colorTexture;
	}

	void ForwardBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		bind();
		target->setRenderResolution();

		BlendGuard blend(4);
		blend.blendAlpha();

#if DIFFUSE_SPECULAR_SEPARATION
		BlendGuard blendSpecular(5);
		blendSpecular.blendAlpha();
#endif

		drawCall();

		unbind();
	}

	Resolution ForwardBuffer::getResolution() const {
		assert(target != nullptr);
		return target->getRenderResolution();
	}


	std::string ForwardBuffer::toString() const {
		return "FBuffer " + std::to_string(fbo.token) + "\n";
	}


}