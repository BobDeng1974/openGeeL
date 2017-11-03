#define GLEW_STATIC
#include <glew.h>
#include "utility/viewport.h"
#include "primitives/screenquad.h"
#include "utility/glguards.h"
#include "stackbuffer.h"
#include "tbuffer.h"

namespace geeL {


	TransparentBuffer::TransparentBuffer(GBuffer& gBuffer, StackBuffer& stackBuffer) : gBuffer(gBuffer), 
		stackBuffer(stackBuffer), compositionTexture(nullptr), tComp("renderer/framebuffer/tcomp.frag") {
		
		this->resolution = gBuffer.getResolution();

		accumulationTexture = new RenderTexture(resolution, ColorType::RGBA16);
		revealageTexture = new RenderTexture(resolution, ColorType::Single);
	}

	TransparentBuffer::~TransparentBuffer() {
		delete accumulationTexture;
		delete revealageTexture;
	}


	void TransparentBuffer::init(RenderTexture& colorTexture) {
		compositionTexture = &colorTexture;

		tComp.init(ScreenQuad::get(), stackBuffer, resolution);
		tComp.addTextureSampler(*accumulationTexture, "accumulation");
		tComp.addTextureSampler(*revealageTexture, "revealage");

		glGenFramebuffers(1, &fbo.token);
		bind();

		//Create attachements for all color buffers
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer.getPositionRoughness().getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer.getNormalMetallic().getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer.getDiffuse().getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, accumulationTexture->getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, revealageTexture->getID(), 0);


		unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(5, attachments);

		//Create a renderbuffer object for depth and stencil attachment
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution.getWidth(), resolution.getHeight());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		unbind();
	}

	void TransparentBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		bind();
		glDepthMask(GL_FALSE);

		//Clear only accumulation and revealage textures since others are still needed later on
		float clear[4] = { 0.f, 0.f, 0.f, 0.f };
		glClearBufferfv(GL_COLOR, 3, clear);
		glClearBufferfv(GL_COLOR, 4, clear);

		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());

		BlendGuard accuBlend(3);
		accuBlend.blendAdd();

		BlendGuard revBlend(4);
		revBlend.blendReverseAlpha();

		drawCall();

		glDepthMask(GL_TRUE);
		unbind();
	}

	void TransparentBuffer::composite() {
		glDisable(GL_DEPTH_TEST);

		BlendGuard blend;
		blend.blendAdd();

		stackBuffer.push(*compositionTexture);
		stackBuffer.fill([this]() {
			tComp.draw();
		}, clearNothing);

		glEnable(GL_DEPTH_TEST);
	}


	std::string TransparentBuffer::toString() const {
		return "TBuffer " + std::to_string(fbo.token) + "\n";
	}

}