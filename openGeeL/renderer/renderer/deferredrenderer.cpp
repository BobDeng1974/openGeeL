#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <map>
#include "renderscene.h"
#include "window.h"
#include "inputmanager.h"
#include "shader/sceneshader.h"
#include "primitives/screenquad.h"
#include "texturing/texture.h"
#include "texturing/rendertexture.h"
#include "texturing/dynamictexture.h"
#include "texturing/imagetexture.h"
#include "framebuffer/gbuffer.h"
#include "framebuffer/tbuffer.h"
#include "postprocessing/drawdefault.h"
#include "postprocessing/ssao.h"
#include "postprocessing/additiveeffect.h"
#include "cameras/camera.h"
#include "lights/lightmanager.h"
#include "guirenderer.h"
#include "lighting/deferredlighting.h"
#include "utility/viewport.h"
#include "utility/glguards.h"
#include "application.h"
#include "deferredrenderer.h"

using namespace std;

namespace geeL {

	DeferredRenderer::DeferredRenderer(RenderWindow& window, SceneRender& lighting,
		RenderContext& context, DefaultPostProcess& def, GBuffer& gBuffer)
			: Renderer(window, context), gBuffer(gBuffer), ssao(nullptr), fBuffer(nullptr), tBuffer(nullptr), 
				lighting(lighting), toggle(0), defaultEffect(def), fallbackEffect("renderer/shaders/screen.frag") {

		init();
	}

	DeferredRenderer::~DeferredRenderer() {
		delete texture1;
		delete texture2;
	}

	void DeferredRenderer::init() {
		geometryPassFunction = [this] () { this->scene->drawDefault(); };
		lightingPassFunction = [this] () { this->lightingPass(); };

		texture1 = new RenderTexture(window->resolution, ColorType::RGBA16, WrapMode::ClampEdge, FilterMode::None);
		texture2 = new RenderTexture(window->resolution, ColorType::RGBA16, WrapMode::ClampEdge, FilterMode::None);
		defTexture = texture1;

		stackBuffer.initResolution(window->resolution);
		stackBuffer.referenceRBO(gBuffer);

		addRequester(lighting);
		ScreenQuad& defQuad = ScreenQuad::get();
		defQuad.init();
	}

	
	void DeferredRenderer::runStart() {
		window->makeCurrent();

		defaultEffect.init(PostProcessingParameter(ScreenQuad::get(), stackBuffer, window->resolution, &fallbackEffect));
		lighting.init(PostProcessingParameter(ScreenQuad::get(), stackBuffer, window->resolution));

		scene->updateProbes(); //Draw reflection probes once at beginning
		indexEffects();
	}

	void DeferredRenderer::run() {
		draw();
		window->swapBuffer();
	}

	void DeferredRenderer::draw() {
		DepthGuard::enable(true);

		Viewport::setForced(0, 0, window->resolution.getWidth(), window->resolution.getHeight());
		scene->lock();


		for (auto it(renderTextures.begin()); it != renderTextures.end(); it++) {
			DynamicRenderTexture& texture = **it;
			texture.update([this](const Camera& camera) { drawForward(camera); });
		}

		scene->updateCamera();
		scene->getLightmanager().update(*scene, &scene->getCamera());

		//Geometry pass
		gBuffer.fill(geometryPassFunction);

		//Hacky: Read camera depth from geometry pass and write it into the scene
		scene->forwardScreenInfo(gBuffer.screenInfo);

		//Occlusion pass
		if (ssao != nullptr) {
			DepthGuard guard(true);

			stackBuffer.push(gBuffer.requestOcclusion());
			ssao->fill();
		}

		DepthGuard::enable(false);

		//Lighting pass
		stackBuffer.push(*texture1);
		stackBuffer.fill(lightingPassFunction, clearColor);
		
		RenderTexture* last = drawEffects(externalEffects, texture1);
		last = drawEffects(earlyEffects, last);

		//Forward pass
		if (hasForwardPass()) {
			DepthGuard::enable(true);

			fBuffer->fill([this]() {
				scene->drawSkybox();
				scene->drawForward();
				scene->drawTransparentOD();
			});
		}

		DepthGuard::enable(false);
		last = drawEffects(intermediateEffects, last);

		//Generic pass
		if (scene->count(ShadingMethod::Generic) > 0) {
			DepthGuard::enable(true);

			stackBuffer.push(*last);
			stackBuffer.fill([this]() {
				scene->drawGeneric();
			}, clearNothing);
		}

		DepthGuard::enable(false);
		drawEffects(lateEffects, last);

		//Draw the last (default) effect to screen.
		defaultEffect.draw();

		scene->unlock();

		//Render GUI overlay on top of final image
		if (gui != nullptr) gui->draw();

	}


	void DeferredRenderer::draw(const Camera& camera, const FrameBuffer& buffer) {
		DepthGuard::enable(true);

		//Geometry pass
		gBuffer.fill([this, &camera] () { scene->drawDefault(camera); });

		scene->getLightmanager().update(*scene, nullptr);

		//Occlusion pass
		if (ssao != nullptr) {
			ssao->setCamera(camera);
			stackBuffer.push(gBuffer.requestOcclusion());
			stackBuffer.fill(*ssao);
			ssao->updateCamera(scene->getCamera());
		}

		buffer.resetSize();
		buffer.bind();

		//Draw lighting pass and skybox directly into given framebuffer
		//Set custom camera for deferred lighting
		lighting.setCamera(camera);
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
		addRequester(*this->ssao);

		Resolution ssaoRes = Resolution(window->resolution, ssao.getResolution());
		gBuffer.requestOcclusion(ssao.getResolution()); //Ensure that occlusion map gets created
		ssao.init(PostProcessingParameter(ScreenQuad::get(), stackBuffer, ssaoRes, &fallbackEffect));
		ssao.setTargetTexture(*gBuffer.getOcclusion());
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect, DrawTime time) {
		SSAO* ssao = dynamic_cast<SSAO*>(&effect);
		if (ssao != nullptr) {
			addEffect(*ssao);
			return;
		}

		effect.init(PostProcessingParameter(ScreenQuad::get(), stackBuffer,
			window->resolution, &fallbackEffect));


		switch (time) {
			case DrawTime::Early:
				earlyEffects.push_back(PostEffectRender(nullptr, &effect));
				break;
			case DrawTime::Intermediate:
				intermediateEffects.push_back(PostEffectRender(nullptr, &effect));
				break;
			case DrawTime::Late:
				lateEffects.push_back(PostEffectRender(nullptr, &effect));
				break;
		}
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect, RenderTexture& texture) {
		externalEffects.push_back(PostEffectRender(&texture, &effect));
		effect.init(PostProcessingParameter(ScreenQuad::get(), stackBuffer, window->resolution, &fallbackEffect));
	}


	void DeferredRenderer::addRenderTexture(DynamicRenderTexture& texture) {
		renderTextures.push_back(&texture);
	}

	void DeferredRenderer::addRequester(WorldMapRequester& requester) {
		this->requester.push_back(&requester);
	}


	void DeferredRenderer::linkInformation() const {
		//Link world maps to requesting post effects
		map<WorldMaps, const Texture*> worldMaps = std::move(getMaps());

		for (auto it = requester.begin(); it != requester.end(); it++) {
			WorldMapRequester* req = *it;
			req->addWorldInformation(worldMaps);
		}
	}



	map<WorldMaps, const Texture*> DeferredRenderer::getMaps() const {
		map<WorldMaps, const Texture*> worldMaps;
		worldMaps[WorldMaps::Image] = texture1;
		worldMaps[WorldMaps::Diffuse] = &gBuffer.getDiffuse();
		worldMaps[WorldMaps::PositionRoughness] = &gBuffer.getPositionRoughness();
		worldMaps[WorldMaps::NormalMetallic] = &gBuffer.getNormalMetallic();

		const RenderTexture* emissivity = gBuffer.getEmissivity();
		if (emissivity != nullptr)
			worldMaps[WorldMaps::Emissivity] = emissivity;

		const RenderTexture* occlusion = gBuffer.getOcclusion();
		if (occlusion != nullptr)
			worldMaps[WorldMaps::Occlusion] = occlusion;

		return worldMaps;
	}

	void DeferredRenderer::addFBuffer(ForwardBuffer& buffer) {
		fBuffer = &buffer;
	}

	void DeferredRenderer::addTBuffer(TransparentOIDBuffer& buffer) {
		tBuffer = &buffer;
		tBuffer->init(*texture1);
	}

	void DeferredRenderer::setScreenImage(const Texture* const texture) {
		defaultEffect.setImage((texture != nullptr) ? *texture : *defTexture);
	}

	std::vector<const Texture*> DeferredRenderer::getBuffers() {
		const RenderTexture* emisTex = gBuffer.getEmissivity();
		const RenderTexture* occTex = gBuffer.getOcclusion();

		size_t bufferSize = 4;
		bufferSize += int(emisTex != nullptr);
		bufferSize += int(occTex  != nullptr);

		std::vector<const Texture*> buffers;
		buffers.reserve(bufferSize);

		buffers.push_back(&gBuffer.getDiffuse());
		buffers.push_back(&gBuffer.getNormalMetallic());

		if (emisTex != nullptr)
			buffers.push_back(emisTex);
		
		if (occTex != nullptr)
			buffers.push_back(occTex);

		buffers.push_back(texture1);
		buffers.push_back(texture2);

		return buffers;
	}

	StackBuffer& DeferredRenderer::getStackbuffer() {
		return stackBuffer;
	}



	void DeferredRenderer::indexEffects() {
		RenderTexture* lastImage = indexEffectList(earlyEffects, texture1);

		if(fBuffer != nullptr)
			fBuffer->setColorTexture(*lastImage);

		lastImage = indexEffectList(intermediateEffects, lastImage);
		lastImage = indexEffectList(lateEffects, lastImage);

		defTexture = lastImage;
		defaultEffect.setImage(*defTexture);
	}

	RenderTexture* DeferredRenderer::indexEffectList(std::vector<PostEffectRender>& effects, RenderTexture* firstTexture) {
		RenderTexture* last = firstTexture;

		for (auto it(effects.begin()); it != effects.end(); it++) {
			PostEffectRender& current = *it;

			if (it == effects.begin())
				indexEffect(current, nullptr, firstTexture);
			else {
				PostEffectRender& previous = *prev(it);
				indexEffect(current, &previous, firstTexture);
			}

			last = current.first;
		}

		return last;
	}

	RenderTexture* DeferredRenderer::drawEffects(std::vector<PostEffectRender>& effects, RenderTexture* lastTexture) {
		for (auto it(effects.begin()); it != effects.end(); it++) {
			PostProcessingEffect& effect = *it->second;
			RenderTexture& texture = *it->first;

			stackBuffer.push(texture);
			effect.fill();
		}

		if (effects.size() > 0) {
			PostEffectRender& last = effects.back();
			return last.first;

		}
		
		return lastTexture;
	}

	void DeferredRenderer::indexEffect(PostEffectRender& current, PostEffectRender* previous, RenderTexture* firstTexture) {
		PostProcessingEffect& effect = *current.second;

		//Filter out additive effects and resuse previous texture target
		AdditiveEffect* add = dynamic_cast<AdditiveEffect*>(&effect);
		if (add != nullptr) {
			RenderTexture* target = nullptr;

			if (previous == nullptr)
				target = firstTexture;
			else
				target = previous->first;

			current.first = target;
			effect.setImage(*target);
		}
		//Init all normal post processing effects with two alternating textures
		//Current effect will then always read from one and write to the other
		else {
			const Texture* source = nullptr;
			RenderTexture* target = nullptr;

			if (previous == nullptr) {
				source = firstTexture;
				target = (firstTexture == texture1) ? texture2 : texture1;
			}
			else {
				source = (previous->first == texture2) ? texture2 : texture1;
				target = (previous->first == texture2) ? texture1 : texture2;
			}

			current.first = target;
			effect.setImage(*source);
		}

	}

}