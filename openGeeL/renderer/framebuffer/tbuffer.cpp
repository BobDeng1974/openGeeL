#define GLEW_STATIC
#include <glew.h>
#include "utility/viewport.h"
#include "primitives/screenquad.h"
#include "utility/glguards.h"
#include "stackbuffer.h"
#include "tbuffer.h"

namespace geeL {


	TransparentOIDBuffer::TransparentOIDBuffer(GBuffer& gBuffer, DynamicBuffer& compBuffer) : gBuffer(gBuffer), 
		compBuffer(compBuffer), compositionTexture(nullptr), tComp("renderer/framebuffer/tcomp.frag") {
		
		this->resolution = gBuffer.getResolution();

		accumulationTexture = new RenderTexture(resolution, ColorType::RGBA16);
		revealageTexture = new RenderTexture(resolution, ColorType::Single);
	}

	TransparentOIDBuffer::~TransparentOIDBuffer() {
		delete accumulationTexture;
		delete revealageTexture;
	}


	void TransparentOIDBuffer::init(RenderTexture& colorTexture) {
		compositionTexture = &colorTexture;

		tComp.init(PostProcessingParameter(ScreenQuad::get(), compBuffer, resolution));
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

		referenceRBO(gBuffer);

		unbind();
	}

	void TransparentOIDBuffer::fill(std::function<void()> drawCall, Clearer clearer) {
		bind();
		glDepthMask(GL_FALSE);

		//Clear only accumulation and revealage textures since others 
		//contain information that is still needed later on
		float clearAcc[4] = { 0.f, 0.f, 0.f, 0.f };
		float clearRev[4] = { 1.f, 0.f, 0.f, 0.f };
		glClearBufferfv(GL_COLOR, 3, clearAcc);
		glClearBufferfv(GL_COLOR, 4, clearRev);

		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());

		BlendGuard accuBlend(3);
		accuBlend.blendAdd();

		BlendGuard revBlend(4);
		revBlend.blendUnder();

		drawCall();

		glDepthMask(GL_TRUE);
		unbind();
	}

	void TransparentOIDBuffer::composite() {
		DepthGuard depth(true);
		BlendGuard blend;
		blend.blendAlpha();

		compBuffer.push(*compositionTexture);
		compBuffer.fill([this]() {
			tComp.draw();
		}, clearNothing);

	}


	std::string TransparentOIDBuffer::toString() const {
		return "TBuffer " + std::to_string(fbo.token) + "\n";
	}

}