#include <glfw3.h>
#include <algorithm>
#include <iostream>
#include "renderscene.h"
#include "window.h"
#include "primitives/screenquad.h"
#include "texturing/textureprovider.h"
#include "texturing/dynamictexture.h"
#include "framebuffer/gbuffer.h"
#include "postprocessing/drawdefault.h"
#include "postprocessing/ssao.h"
#include "cameras/camera.h"
#include "lights/lightmanager.h"
#include "lighting/deferredlighting.h"
#include "glwrapper/viewport.h"
#include "glwrapper/glguards.h"
#include "guirenderer.h"
#include "application.h"
#include "deferredrenderer.h"

using namespace std;

namespace geeL {

	DeferredRenderer::DeferredRenderer(
		RenderWindow& window, 
		TextureProvider& provider, 
		SceneRender& lighting,
		RenderContext& context, 
		DefaultPostProcess& def, 
		GBuffer& gBuffer)
			: Renderer(window, context)
			, provider(provider)
			, gBuffer(gBuffer)
			, ssao(nullptr)
			, fBuffer(nullptr)
			, lighting(lighting)
			, defaultEffect(def)
			, fallbackEffect("shaders/screen.frag")
			, combineEffect("shaders/screen.frag") {

		init();
	}


	void DeferredRenderer::init() {
		geometryPassFunction = [this] () { this->scene->drawDefault(); };
		lightingPassFunction = [this] () { this->lightingPass(); };

		stackBuffer.initResolution(window->resolution);
		stackBuffer.referenceRBO(gBuffer);

#if DIFFUSE_SPECULAR_SEPARATION
		separatedBuffer.initResolution(window->resolution);
		separatedBuffer.referenceRBO(gBuffer);
		separatedBuffer.setSize(2);
#endif

		ScreenQuad& defQuad = ScreenQuad::get();
		defQuad.init();
	}

	
	void DeferredRenderer::runStart() {
		window->makeCurrent();
		
		initEffects();
		scene->updateProbes(); //Draw reflection probes once at beginning
	}

	void DeferredRenderer::run() {
		draw();

		provider.cleanupCache();
		window->swapBuffer();
	}

	void DeferredRenderer::draw() {
		DepthGuard::enable(true);
		Viewport::setForced(0, 0, window->resolution.getWidth(), window->resolution.getHeight());

		//Update scene and forward information into objects and effects
		
		scene->lock();

		for (auto it(renderTextures.begin()); it != renderTextures.end(); it++) {
			DynamicRenderTexture& texture = **it;
			texture.update([this](const Camera& camera) { drawForward(camera); });
		}

		scene->updateCamera();
		scene->getLightmanager().update(*scene, &scene->getCamera());
		scene->updateBindings();
		updateEffectBindings();

		scene->unlock();

		//Draw all objects and effects

		//Geometry pass
		gBuffer.fill(geometryPassFunction);

		//Occlusion pass
		if (ssao != nullptr) {
			DepthGuard guard(true);
			ssao->fill();
		}

		DepthGuard::enable(false);

		//Lighting pass
#if DIFFUSE_SPECULAR_SEPARATION
		//TODO: draw skybox
		lighting.fill();
#else
		stackBuffer.push(provider.requestCurrentImage());
		stackBuffer.fill(lightingPassFunction, clearColor);
#endif

		drawEffects(externalEffects);
		drawEffects(earlyEffects);

		//Forward pass
		if (hasForwardPass()) {
			DepthGuard::enable(true);

#if DIFFUSE_SPECULAR_SEPARATION
			RenderTexture& diffuse  = provider.requestCurrentImage();
			RenderTexture& specular = provider.requestCurrentSpecular();
			LayeredTarget combinedTarget(diffuse, specular);

			fBuffer->setTarget(combinedTarget);
#else
			fBuffer->setTarget(provider.requestCurrentImage());
#endif
			fBuffer->fill([this]() {
				scene->drawSkybox();
				scene->drawForward();
				scene->drawTransparentOD();
			});
		}

		DepthGuard::enable(false);
		drawEffects(intermediateEffects);

#if DIFFUSE_SPECULAR_SEPARATION
		combineEffect.setImage(provider.requestCurrentSpecular());
		combineEffect.fill();
#endif

		//Generic pass
		if (scene->contains(ShadingMethod::Generic)) {
			DepthGuard::enable(true);

			stackBuffer.push(provider.requestCurrentImage());
			stackBuffer.fill([this]() {
				scene->drawGeneric();
			}, clearNothing);
		}

		DepthGuard::enable(false);
		drawEffects(lateEffects);

		//Draw the last (default) effect to screen.
		defaultEffect.draw();


		//Render GUI overlay on top of final image
		if (gui != nullptr) {
			scene->lock();
			gui->draw();
			scene->unlock();
		}

	}


	void DeferredRenderer::draw(const Camera& camera, const FrameBuffer& buffer) {
		DepthGuard::enable(true);

		//Geometry pass
		gBuffer.fill([this, &camera] () { scene->drawDefault(camera); });

		scene->getLightmanager().update(*scene, nullptr);

		//Occlusion pass
		if (ssao != nullptr) {
			ssao->setCamera(camera);
			ssao->fill();
			ssao->updateCamera(scene->getCamera());
		}

		buffer.setRenderResolution();
		buffer.bind();

		//Draw lighting pass and skybox directly into given framebuffer
		//Set custom camera for deferred lighting
		lighting.setCamera(camera);
		lighting.bindValues();
		lighting.draw();
		lighting.setCamera(scene->getCamera());

		scene->drawSkybox(camera);
	}

	void DeferredRenderer::drawForward(const Camera& camera) {
		DepthGuard::enable(true);
		
		scene->drawGenericForced(camera, true);
		scene->drawSkybox(camera);
	}



	void DeferredRenderer::lightingPass() {
		lighting.draw();

		//Draw skybox directly alongside the lighting
		//if forward rendering is deactivated
		if (!hasForwardPass()) {
			DepthGuard depth;
			scene->drawSkybox();
		}
	}

	bool DeferredRenderer::hasForwardPass() const {
		return fBuffer != nullptr && scene->contains(ShadingMethod::Forward, ShadingMethod::TransparentOD);
	}



	void DeferredRenderer::addEffect(SSAO& ssao) {
		this->ssao = &ssao;

		Resolution ssaoRes = Resolution(window->resolution, ssao.getResolution());
		gBuffer.requestOcclusion(ssao.getResolution()); //Ensure that occlusion map gets created

		ssao.init(PostProcessingParameter(ScreenQuad::get(), stackBuffer, 
			ssaoRes, &provider, &fallbackEffect));
		ssao.setTargetTexture(*gBuffer.getOcclusion());
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect, DrawTime time) {
		SSAO* ssao = dynamic_cast<SSAO*>(&effect);
		if (ssao != nullptr) {
			addEffect(*ssao);
			return;
		}

		switch (time) {
			case DrawTime::Early:
				earlyEffects.push_back(&effect);
				break;
			case DrawTime::Intermediate:
				intermediateEffects.push_back(&effect);
				break;
			case DrawTime::Late:
				lateEffects.push_back(&effect);
				break;
		}
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect, RenderTexture& texture) {
		externalEffects.push_back(&effect);
	}


	void DeferredRenderer::addRenderTexture(DynamicRenderTexture& texture) {
		renderTextures.push_back(&texture);
	}

	void DeferredRenderer::addFBuffer(ForwardBuffer& buffer) {
		fBuffer = &buffer;
	}

	void DeferredRenderer::setScreenImage(const Texture* const texture) {
		defaultEffect.setCustomImage(texture);
	}

	const TextureProvider& DeferredRenderer::getTextureProvider() const {
		return provider;
	}




	void DeferredRenderer::initEffects() {
#if DIFFUSE_SPECULAR_SEPARATION
		PostProcessingParameter parameter(ScreenQuad::get(), stackBuffer,
			window->resolution, &provider, &fallbackEffect, &separatedBuffer);
#else
		PostProcessingParameter parameter(ScreenQuad::get(), stackBuffer,
			window->resolution, &provider, &fallbackEffect);
#endif

		defaultEffect.init(parameter);
		lighting.init(parameter);
		combineEffect.init(parameter);

		iterEffects(externalEffects, [this, &parameter](PostProcessingEffect& effect) { 
			effect.init(parameter); });

		iterEffects(earlyEffects, [this, &parameter](PostProcessingEffect& effect) {
			effect.init(parameter); });

		iterEffects(intermediateEffects, [this, &parameter](PostProcessingEffect& effect) {
			effect.init(parameter); });

		iterEffects(lateEffects, [this, &parameter](PostProcessingEffect& effect) {
			effect.init(parameter); });
	}



	void DeferredRenderer::drawEffects(std::vector<PostProcessingEffect*>& effects) {
		for (auto it(effects.begin()); it != effects.end(); it++) {
			PostProcessingEffect& effect = **it;
			effect.fill();
		}
	}

	void DeferredRenderer::updateEffectBindings() {
		lighting.bindValues();
		defaultEffect.bindValues();

		iterEffects(externalEffects, [](PostProcessingEffect& effect) { effect.bindValues(); });
		iterEffects(earlyEffects, [](PostProcessingEffect& effect) { effect.bindValues(); });
		iterEffects(intermediateEffects, [](PostProcessingEffect& effect) { effect.bindValues(); });
		iterEffects(lateEffects, [](PostProcessingEffect& effect) { effect.bindValues(); });
	}


	void DeferredRenderer::iterEffects(std::vector<PostProcessingEffect*>& effects,
		std::function<void(PostProcessingEffect&)> function) {
		
		for (auto it(effects.begin()); it != effects.end(); it++)
			function(**it);
	}

}