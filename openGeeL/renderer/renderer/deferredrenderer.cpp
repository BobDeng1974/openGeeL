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
		RenderScene& scene,
		MeshFactory& meshFactory,
		MaterialFactory& materialFactory)
			: Renderer(window, context, meshFactory, materialFactory)
			, scene(scene)
			, provider(provider)
			, gBuffer(gBuffer)
			, ssao(nullptr)
			, fBuffer(nullptr)
			, lighting(lighting)
			, defaultEffect(def)
			, fallbackEffect("shaders/screen.frag")
			, combineEffect("shaders/screen.frag")
			, sceneReady(false) {

		init();
	}


	void DeferredRenderer::init() {
		geometryPassFunction = [this] () { this->scene.draw(ShadingMethod::Deferred); };

		stackBuffer.initResolution(provider.getRenderResolution());
		stackBuffer.referenceRBO(gBuffer);

#if DIFFUSE_SPECULAR_SEPARATION
		separatedBuffer.initResolution(provider.getRenderResolution());
		separatedBuffer.referenceRBO(gBuffer);
		separatedBuffer.setSize(2);
#endif

		ScreenQuad& defQuad = ScreenQuad::get();
		defQuad.init();

		scene.addUpdateListener([this](RenderScene& scene) {
			this->sceneReady = true;
		});
	}

	
	void DeferredRenderer::runStart() {
		Renderer::runStart();

		lock_guard<mutex> glGuard(glMutex);
		lock_guard<mutex> renderGuard(renderMutex);
		
		initEffects();
		scene.updateProbes(); //Draw reflection probes once at beginning
	}

	void DeferredRenderer::run() {
		Renderer::run();
		provider.cleanupCache();

		//Only draw current state of scene if it has actually updates since last draw call
		if (sceneReady) sceneReady = false;
		else return;

		lock_guard<mutex> glGuard(glMutex);
		lock_guard<mutex> renderGuard(renderMutex);
		draw();

		provider.swap();
		
		window.swapBuffer();
	}

	void DeferredRenderer::draw() {
		//Update scene and forward information into objects and effects
		
		scene.lock();

		for (auto it(renderTextures.begin()); it != renderTextures.end(); it++) {
			DynamicRenderTexture& texture = **it;
			texture.update([this](const Camera& camera) { drawForward(camera, true); });
		}

		scene.updateCamera();
		scene.getLightmanager().update(scene, &scene.getCamera());
		scene.updateBindings();
		updateEffectBindings();

		scene.unlock();

		//Draw all objects and effects
		DepthGuard::enable(true);
		Viewport::setForced(0, 0, provider.getRenderResolution().getWidth(), provider.getRenderResolution().getHeight());

		//Geometry pass
		gBuffer.fill(geometryPassFunction);

		//Occlusion pass
		if (ssao != nullptr) {
			DepthGuard guard(true);
			ssao->fill();
		}

		DepthGuard::enable(false);
		lighting.fill();

		//Draw skybox
		stackBuffer.push(provider.requestCurrentImage());
		stackBuffer.fill([this]() {
			DepthGuard depth;
			scene.drawSkybox();
		}, clearNothing);


		drawEffects(earlyEffects);
		drawEffects(externalEffects);

		//Hyprid forward pass (Hyprid & transparent objects
		if (hasForwardPass()) {
			DepthGuard::enable(true);

#if DIFFUSE_SPECULAR_SEPARATION
			RenderTarget& diffuse  = provider.requestCurrentImage();
			RenderTarget& specular = provider.requestCurrentSpecular();
			LayeredTarget combinedTarget(diffuse, specular);

			fBuffer->setTarget(combinedTarget);
#else
			fBuffer->setTarget(provider.requestCurrentImage());
#endif
			fBuffer->fill([this]() {
				scene.draw(ShadingMethod::Hybrid);

				DepthWriteGuard depthWriteDisable;
				scene.draw(ShadingMethod::Transparent);
			});
		}

		DepthGuard::enable(false);
		drawEffects(intermediateEffects);

#if DIFFUSE_SPECULAR_SEPARATION
		combineEffect.setImage(provider.requestCurrentSpecular());
		combineEffect.fill();
#endif

		//Generic forward pass
		if (scene.contains(ShadingMethod::Forward)) {
			DepthGuard::enable(true);

			stackBuffer.push(provider.requestCurrentImage());
			stackBuffer.fill([this]() {
				scene.draw(ShadingMethod::Forward);
			}, clearNothing);
		}

		DepthGuard::enable(false);
		drawEffects(lateEffects);

		//Draw the last (default) effect to screen.
		defaultEffect.draw();

		//Render GUI overlay on top of final image
		if (gui != nullptr) {
			scene.lock();
			gui->draw();
			scene.unlock();
		}

	}


	void DeferredRenderer::drawForward(const Camera& camera, bool forceGamma) const {
		DepthGuard::enable(true);
		
		scene.drawForwardForced(camera, forceGamma);
		scene.drawSkybox(camera);
	}


	bool DeferredRenderer::hasForwardPass() const {
		return fBuffer != nullptr && scene.contains(ShadingMethod::Hybrid, ShadingMethod::Transparent);
	}



	void DeferredRenderer::addEffect(SSAO& ssao) {
		this->ssao = &ssao;

		ssao.init(PostProcessingParameter(ScreenQuad::get(), stackBuffer, 
			provider.getRenderResolution(), &provider, &fallbackEffect, nullptr, &materialFactory));
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
			case DrawTime::External:
				externalEffects.push_back(&effect);
				break;
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

	TextureProvider& DeferredRenderer::getTextureProvider() const {
		return provider;
	}


	void DeferredRenderer::initEffects() {
#if DIFFUSE_SPECULAR_SEPARATION
		PostProcessingParameter parameter(ScreenQuad::get(), stackBuffer,
			provider.getRenderResolution(), &provider, &fallbackEffect, &separatedBuffer, &materialFactory);
#else
		PostProcessingParameter parameter(ScreenQuad::get(), stackBuffer,
			provider.getRenderResolution(), &provider, &fallbackEffect, nullptr, &materialFactory);
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