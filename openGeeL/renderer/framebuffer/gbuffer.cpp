#define GLEW_STATIC
#include <glew.h>
#include <climits>
#include <iostream>
#include "glwrapper/viewport.h"
#include "glwrapper/glguards.h"
#include "shader/rendershader.h"
#include "renderer/renderer.h"
#include "texturing/texturetarget.h"
#include "appglobals.h"
#include "bufferutil.h"

#include "gbuffer.h"

namespace geeL {

	GBuffer::GBuffer(Resolution resolution, GBufferContent content) 
		: content(content)
		, resolution(resolution)
		, emissivity(nullptr)
		, occlusion(nullptr) {
		
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
		
		unbind();
	}

	float GBuffer::getDepth() const {
		return depthPos;
	}

	Resolution GBuffer::getResolution() const {
		return resolution;
	}

	const RenderTarget& GBuffer::getDiffuse() const {
		return *diffuse;
	}

	const RenderTarget& GBuffer::getPositionRoughness() const {
		return *positionRough;
	}

	const RenderTarget& GBuffer::getNormalMetallic() const {
		return *normalMet;
	}

	const RenderTarget* GBuffer::getEmissivity() const {
		return emissivity;
	}

	const RenderTarget* GBuffer::getOcclusion() const {
		return occlusion;
	}

	RenderTarget& GBuffer::requestOcclusion(const ResolutionScale& scale) {
		if (occlusion == nullptr)
			occlusion = TextureTarget::createTextureTargetPtr<Texture2D>(
				Resolution(resolution, scale), ColorType::Single, FilterMode::None, WrapMode::ClampEdge).release();

		return *occlusion;
	}

	std::string GBuffer::getFragmentPath() const {
		switch (content) {
			case GBufferContent::DefaultEmissive:
				return "shaders/gbufferEmi.frag";
			default:
				return "shaders/gbuffer.frag";
		}
	}

	std::string GBuffer::toString() const {
		return "GBuffer " + std::to_string(fbo.token) + "\n";
	}

	void GBuffer::initTextures(Resolution resolution) {
		positionRough = TextureTarget::createTextureTargetPtr<Texture2D>(resolution, ColorType::RGBA16).release();
		normalMet = TextureTarget::createTextureTargetPtr<Texture2D>(resolution, ColorType::RGBA16).release();
		diffuse = TextureTarget::createTextureTargetPtr<Texture2D>(resolution, ColorType::RGBA).release();
		
		positionRough->assignTo(*this, 0);
		normalMet->assignTo(*this, 1);
		diffuse->assignTo(*this, 2);
		
		switch (content) {
			case GBufferContent::DefaultEmissive: {
				emissivity = TextureTarget::createTextureTargetPtr<Texture2D>(resolution, ColorType::RGB).release();
				occlusion = TextureTarget::createTextureTargetPtr<Texture2D>(
					resolution, ColorType::Single, FilterMode::None, WrapMode::ClampEdge).release();

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

	ForwardBuffer::ForwardBuffer(GBuffer& gBuffer) 
		: gBuffer(gBuffer)
		, target(nullptr) {

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

#if DIFFUSE_SPECULAR_SEPARATION
		BufferUtility::drawBuffers(5);
#else
		BufferUtility::drawBuffers(4);
#endif

		referenceRBO(gBuffer);

		unbind();
	}

	void ForwardBuffer::setTarget(RenderTarget& colorTexture) {
		bind();
		colorTexture.assignTo(*this, 3);
		target = &colorTexture;
	}

	void ForwardBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		bind();
		target->setRenderResolution();

		BlendGuard blend(3);
		blend.blendAlpha();

#if DIFFUSE_SPECULAR_SEPARATION
		BlendGuard blendSpecular(4);
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