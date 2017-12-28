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
		GBuffer& gBuffer,
		MeshFactory& meshFactory,
		MaterialFactory& materialFactory)
			: Renderer(window, context, meshFactory, materialFactory)
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

		stackBuffer.initResolution(window.resolution);
		stackBuffer.referenceRBO(gBuffer);

#if DIFFUSE_SPECULAR_SEPARATION
		separatedBuffer.initResolution(window.resolution);
		separatedBuffer.referenceRBO(gBuffer);
		separatedBuffer.setSize(2);
#endif

		ScreenQuad& defQuad = ScreenQuad::get();
		defQuad.init();
	}

	
	void DeferredRenderer::runStart() {
		Renderer::runStart();

		lock_guard<mutex> glGuard(glMutex);
		lock_guard<mutex> renderGuard(renderMutex);
		
		initEffects();
		scene->updateProbes(); //Draw reflection probes once at beginning
	}

	void DeferredRenderer::run() {
		Renderer::run();

		lock_guard<mutex> glGuard(glMutex);
		lock_guard<mutex> renderGuard(renderMutex);
		draw();

		provider.cleanupCache();
		window.swapBuffer();
	}

	void DeferredRenderer::draw() {
		DepthGuard::enable(true);
		Viewport::setForced(0, 0, window.resolution.getWidth(), window.resolution.getHeight());

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

		lighting.fill();

		drawEffects(externalEffects);
		drawEffects(earlyEffects);

		//Hyprid forward pass (Hyprid & transparent objects + skybox
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
				scene->drawHybrid();

				DepthWriteGuard depthWriteDisable;
				scene->drawTransparent();
			});
		}
		//Draw skybox only
		else {
			stackBuffer.push(provider.requestCurrentImage());
			stackBuffer.fill([this]() {
				DepthGuard depth;
				scene->drawSkybox();
			}, clearNothing);
		}

		DepthGuard::enable(false);
		drawEffects(intermediateEffects);

#if DIFFUSE_SPECULAR_SEPARATION
		combineEffect.setImage(provider.requestCurrentSpecular());
		combineEffect.fill();
#endif

		//Generic forward pass
		if (scene->contains(ShadingMethod::Forward)) {
			DepthGuard::enable(true);

			stackBuffer.push(provider.requestCurrentImage());
			stackBuffer.fill([this]() {
				scene->drawForward();
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


	bool DeferredRenderer::hasForwardPass() const {
		return fBuffer != nullptr && scene->contains(ShadingMethod::Hybrid, ShadingMethod::Transparent);
	}



	void DeferredRenderer::addEffect(SSAO& ssao) {
		this->ssao = &ssao;

		Resolution ssaoRes = Resolution(window.resolution, ssao.getResolution());
		gBuffer.requestOcclusion(ssao.getResolution()); //Ensure that occlusion map gets created

		ssao.init(PostProcessingParameter(ScreenQuad::get(), stackBuffer, 
			ssaoRes, &provider, &fallbackEffect, nullptr, &materialFactory));
		ssao.setTargetTexture(*gBuffer.getOcclusion());
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect, DrawTime time) {
		lock_guard<mutex> glGuard(glMutex);
		lock_guard<mutex> renderGuard(renderMutex);

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
		lock_guard<mutex> renderGuard(renderMutex);

		externalEffects.push_back(&effect);
	}


	void DeferredRenderer::addRenderTexture(DynamicRenderTexture& texture) {
		lock_guard<mutex> renderGuard(renderMutex);

		renderTextures.push_back(&texture);
	}

	void DeferredRenderer::addFBuffer(ForwardBuffer& buffer) {
		lock_guard<mutex> renderGuard(renderMutex);

		fBuffer = &buffer;
	}

	void DeferredRenderer::setScreenImage(const ITexture* const texture) {
		defaultEffect.setCustomImage(texture);
	}

	const TextureProvider& DeferredRenderer::getTextureProvider() const {
		return provider;
	}


	void DeferredRenderer::initEffects() {
#if DIFFUSE_SPECULAR_SEPARATION
		PostProcessingParameter parameter(ScreenQuad::get(), stackBuffer,
			window.resolution, &provider, &fallbackEffect, &separatedBuffer, &materialFactory);
#else
		PostProcessingParameter parameter(ScreenQuad::get(), stackBuffer,
			window.resolution, &provider, &fallbackEffect, nullptr, &materialFactory);
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