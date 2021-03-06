#define GLEW_STATIC
#include <glew.h>
#include <climits>
#include <iostream>
#include "glwrapper/viewport.h"
#include "glwrapper/glguards.h"
#include "shader/rendershader.h"
#include "renderer/glstructures.h"
#include "texturing/textureprovider.h"
#include "texturing/rendertexture.h"
#include "appglobals.h"
#include "bufferutil.h"
#include "rendertarget.h"
#include "gbuffer.h"


namespace geeL {

	GBuffer::GBuffer(ITextureProvider& provider)
		: provider(provider) {
		
		init();
	}

	void GBuffer::init() {
		const Resolution& resolution = provider.getRenderResolution();

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
		const Resolution& resolution = provider.getRenderResolution();

		bind();
		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());
		clearer.clear();

		drawCall();
		unbind();
	}

	Resolution GBuffer::getResolution() const {
		return provider.getRenderResolution();
	}

	std::string GBuffer::getFragmentPath() const {
		return "shaders/gbuffer.frag";
	}

	std::string GBuffer::toString() const {
		return "GBuffer " + std::to_string(fbo.token) + "\n";
	}

	void GBuffer::initTextures(Resolution resolution) {
		RenderTarget& position = provider.requestPosition();
		RenderTarget& normal = provider.requestNormal();
		RenderTarget& diffuse = provider.requestAlbedo();
		RenderTarget& properties = provider.requestProperties();

		RenderTarget* const emission = provider.requestEmission();


		position.assignTo(*this, 0);
		normal.assignTo(*this, 1);
		diffuse.assignTo(*this, 2);
		properties.assignTo(*this, 3);

		//Emission texture exists and we therefore attach it to gBuffer
		if (emission) {
			emission->assignTo(*this, 4);

			unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, 
				GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
			glDrawBuffers(5, attachments);
		}
		//Ignore it otherwises
		else {
			unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, 
				GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(4, attachments);

		}
		
	}


	ForwardBuffer::ForwardBuffer(GBuffer& gBuffer, ITextureProvider& provider)
		: gBuffer(gBuffer)
		, provider(provider)
		, target(nullptr) {

		init();
	}

	void ForwardBuffer::init() {
		glGenFramebuffers(1, &fbo.token);
		bind();

		//Create attachements for all color buffers
		provider.requestPosition().assignToo(*this, 0);
		provider.requestNormal().assignToo(*this, 1);
		provider.requestAlbedo().assignToo(*this, 2);
		provider.requestProperties().assignToo(*this, 3);

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
		target->applyRenderResolution();

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
		return target->getRenderPreset();
	}


	std::string ForwardBuffer::toString() const {
		return "FBuffer " + std::to_string(fbo.token) + "\n";
	}


}