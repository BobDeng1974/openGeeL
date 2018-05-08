#define GLEW_STATIC
#include <glew.h>
#include "glwrapper/viewport.h"
#include "glwrapper/glguards.h"
#include "primitives/screenquad.h"
#include "texturing/textureprovider.h"
#include "stackbuffer.h"
#include "tbuffer.h"

namespace geeL {


	TransparentOIDBuffer::TransparentOIDBuffer(GBuffer& gBuffer, ITextureProvider& provider, DynamicBuffer& compBuffer) 
		: gBuffer(gBuffer)
		, provider(provider)
		, compBuffer(compBuffer)
		, compositionTexture(nullptr)
		, tComp("shaders/tcomp.frag") {
		
		Resolution res(gBuffer.getResolution());

		accumulationTexture = new RenderTexture(res, ColorType::RGBA16);
		revealageTexture = new RenderTexture(res, ColorType::Single);
	}

	TransparentOIDBuffer::~TransparentOIDBuffer() {
		delete accumulationTexture;
		delete revealageTexture;
	}


	void TransparentOIDBuffer::init(RenderTexture& colorTexture) {
		compositionTexture = &colorTexture;

		Resolution res(gBuffer.getResolution());
		tComp.init(PostProcessingParameter(ScreenQuad::get(), compBuffer, res));
		tComp.addTextureSampler(*accumulationTexture, "accumulation");
		tComp.addTextureSampler(*revealageTexture, "revealage");

		glGenFramebuffers(1, &fbo.token);
		bind();

		//Create attachements for all color buffers
		provider.requestPosition().assignToo(*this, 0);
		provider.requestNormal().assignToo(*this, 1);
		provider.requestAlbedo().assignToo(*this, 2);

		accumulationTexture->assignTo(*this, 3);
		revealageTexture->assignTo(*this, 4);

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

		setRenderResolution();

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
			tComp.bindValues();
			tComp.draw();
		}, clearNothing);

	}

	Resolution TransparentOIDBuffer::getResolution() const {
		return gBuffer.getResolution();
	}


	std::string TransparentOIDBuffer::toString() const {
		return "TBuffer " + std::to_string(fbo.token) + "\n";
	}

}