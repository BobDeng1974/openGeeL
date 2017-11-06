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

	DeferredRenderer::DeferredRenderer(RenderWindow& window, Input& inputManager, SceneRender& lighting,
		RenderContext& context, DefaultPostProcess& def, GBuffer& gBuffer)
			: Renderer(window, inputManager, context), gBuffer(gBuffer), ssao(nullptr), 
				lighting(lighting), toggle(0), defaultEffect(def), fallbackEffect("renderer/shaders/screen.frag") {

		init();
	}

	DeferredRenderer::~DeferredRenderer() {
		if(ssaoTexture != nullptr) delete ssaoTexture;

		delete texture1;
		delete texture2;
	}

	void DeferredRenderer::init() {
		auto func = [this](int key, int scancode, int action, int mode) 
			{ this->handleInput(key, scancode, action, mode); };

		input.addCallback(func);

		geometryPassFunction = [this] () { this->scene->drawDefault(); };
		lightingPassFunction = [this] () { this->lightingPass(); };

		texture1 = new RenderTexture(window->resolution, ColorType::RGBA16, WrapMode::ClampEdge, FilterMode::None);
		texture2 = new RenderTexture(window->resolution, ColorType::RGBA16, WrapMode::ClampEdge, FilterMode::None);

		stackBuffer.initResolution(window->resolution);

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
		glEnable(GL_DEPTH_TEST);

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

		//SSAO pass
		if (ssao != nullptr) {
			DepthGuard guard(true);

			stackBuffer.push(*ssaoTexture);
			stackBuffer.fill(*ssao);

		}

		//Lighting pass
		stackBuffer.push(*texture1);
		stackBuffer.fill(lightingPassFunction);

		//Forward pass
		if (fBuffer != nullptr && scene->count(ShadingMethod::Forward, ShadingMethod::TransparentOD) > 0) {
			fBuffer->fill([this]() {
				scene->drawForward();
				scene->drawTransparentOD();
			});
		}

		glDisable(GL_DEPTH_TEST);

		//Post processing
		//Draw external post processing effects first.
		drawEffects(externalEffects);

		//Draw all the post processing effects on top of each other.
		drawEffects(effects);

		//Draw the last (default) effect to screen.
		defaultEffect.draw();

		scene->unlock();

		//Render GUI overlay on top of final image
		if (gui != nullptr) gui->draw();

	}


	void DeferredRenderer::draw(const Camera& camera, const FrameBuffer& buffer) {
		glEnable(GL_DEPTH_TEST);

		//Geometry pass
		gBuffer.fill([this, &camera] () { scene->drawDefault(camera); });

		scene->getLightmanager().update(*scene, nullptr);

		//SSAO pass
		if (ssao != nullptr) {
			ssao->setCamera(camera);
			stackBuffer.push(*ssaoTexture);
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
		glEnable(GL_DEPTH_TEST);
		scene->drawGenericForced(camera, true);
		scene->drawSkybox(camera);
	}



	void DeferredRenderer::lightingPass() {
		lighting.draw();

		//Copy depth and stencil buffer from gBuffer to draw forward 
		//rendered objects 'into' the scene instead of 'on top'
		stackBuffer.copyRBO(gBuffer);

		scene->drawGeneric();
		scene->drawSkybox();
	}



	void DeferredRenderer::addEffect(SSAO& ssao) {
		this->ssao = &ssao;
		addRequester(*this->ssao);

		Resolution ssaoRes = Resolution(window->resolution, ssao.getResolution());
		ssaoTexture = new RenderTexture(ssaoRes, ColorType::Single, WrapMode::ClampEdge, FilterMode::None);

		ssao.init(PostProcessingParameter(ScreenQuad::get(), stackBuffer, ssaoRes, &fallbackEffect));
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect, DrawTime time) {
		SSAO* ssao = dynamic_cast<SSAO*>(&effect);
		if (ssao != nullptr) {
			addEffect(*ssao);
			return;
		}

		effect.init(PostProcessingParameter(ScreenQuad::get(), stackBuffer,
			window->resolution, &fallbackEffect));

		effects.push_back(PostEffectRender(nullptr, &effect));
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

		const RenderTexture* emissivity = &gBuffer.getEmissivity();
		if (!emissivity->isEmpty())
			worldMaps[WorldMaps::Emissivity] = emissivity;

		if (ssao != nullptr)
			worldMaps[WorldMaps::SSAO] = ssaoTexture;

		return worldMaps;
	}

	void DeferredRenderer::addFBuffer(ForwardBuffer& buffer) {
		fBuffer = &buffer;
		fBuffer->init(*texture1);
	}

	void DeferredRenderer::addTBuffer(TransparentOIDBuffer& buffer) {
		tBuffer = &buffer;
		tBuffer->init(*texture1);
	}

	StackBuffer& DeferredRenderer::getStackbuffer() {
		return stackBuffer;
	}



	void DeferredRenderer::indexEffects() {
		RenderTexture* lastImage = indexEffectList(effects, texture1);

		defaultEffect.setImage(*lastImage);
	}

	RenderTexture* DeferredRenderer::indexEffectList(std::vector<PostEffectRender>& effects, RenderTexture* firstTexture) {
		RenderTexture* last = firstTexture;

		for (auto it(effects.begin()); it != effects.end(); it++) {
			PostEffectRender& current = *it;

			//First element
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

	RenderTexture* DeferredRenderer::drawEffects(std::vector<PostEffectRender>& effects) {
		for (auto it(effects.begin()); it != effects.end(); it++) {
			PostProcessingEffect& effect = *it->second;
			RenderTexture& texture = *it->first;

			stackBuffer.push(texture);
			effect.fill();
		}

		PostEffectRender& last = effects.back();
		return last.first;
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




	void DeferredRenderer::handleInput(int key, int scancode, int action, int mode) {
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
			toggleBuffer(false);
		
		else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
			toggleBuffer(true);

	}

	void DeferredRenderer::toggleBuffer(bool next) {
		std::vector<const Texture*> buffers = { &defaultEffect.getImage(), 
			&gBuffer.getDiffuse(), &gBuffer.getNormalMetallic() };

		const RenderTexture& emisTex = gBuffer.getEmissivity();
		if (!emisTex.isEmpty())
			buffers.push_back(&emisTex);
		if (ssao != nullptr)
			buffers.push_back(ssaoTexture);

		buffers.push_back(texture1);
		buffers.push_back(texture2);

		int max = int(buffers.size());
		int i = next ? 1 : -1;
		toggle = abs((toggle + i) % max);

		const Texture* currBuffer = buffers[0];
		currBuffer = buffers[toggle];

		defaultEffect.setImage(*currBuffer);
	}

}