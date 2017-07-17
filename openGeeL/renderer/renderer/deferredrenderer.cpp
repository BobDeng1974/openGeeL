#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <map>
#include "../shader/sceneshader.h"
#include "../postprocessing/drawdefault.h"
#include "../texturing/texture.h"
#include "../texturing/rendertexture.h"
#include "../texturing/imagetexture.h"
#include "../window.h"
#include "../framebuffer/gbuffer.h"
#include "../scripting/scenecontrolobject.h"
#include "../inputmanager.h"
#include "../postprocessing/ssao.h"
#include "../postprocessing/postprocessing.h"
#include "../cameras/camera.h"
#include "../lights/lightmanager.h"
#include "../scene.h"
#include "../../interface/guirenderer.h"
#include "../lighting/deferredlighting.h"
#include "deferredrenderer.h"

#define fps 10

using namespace std;

namespace geeL {

	DeferredRenderer::DeferredRenderer(RenderWindow& window, InputManager& inputManager, SceneRender& lighting,
		RenderContext& context, DefaultPostProcess& def, GBuffer& gBuffer, const MaterialFactory& factory)
			: Renderer(window, inputManager, context),
				gBuffer(gBuffer), screen(ScreenQuad()), ssao(nullptr), lighting(lighting),
				toggle(0), factory(factory) {

		effects.push_back(&def);
		init();
	}

	DeferredRenderer::~DeferredRenderer() {
		if (ssaoBuffer != nullptr)
			delete ssaoBuffer;

		if (ssaoScreen != nullptr)
			delete ssaoScreen;
	}


	void DeferredRenderer::init() {
		auto func = [this](GLFWwindow* window, int key, int scancode, int action, int mode) 
			{ this->handleInput(window, key, scancode, action, mode); };

		inputManager->addCallback(func);
		inputManager->init(window);

		geometryPassFunc = [this]() { this->geometryPass(); };
		lightingPassFunc = [this]() { this->lightingPass(); this->forwardPass(); };

		stackBuffer.init(window->resolution, ColorType::RGBA16, FilterMode::None, WrapMode::ClampEdge);
		stackBuffer.initDepth(); 

		screen.init();
		addRequester(lighting);
	}


	void DeferredRenderer::initSceneObjects() {
		Renderer::initSceneObjects();

		//Init scene after all scene objects are ready
		scene->init();
	}


	void DeferredRenderer::render() {
		lighting.init(screen, stackBuffer);
		scene->updateProbes(); //Draw reflection probes once at beginning
		initDefaultEffect();

		//Render loop
		while (!window->shouldClose()) {
			int currFPS = (int)ceil(Time::deltaTime * 1000.f);
			this_thread::sleep_for(chrono::milliseconds(fps - currFPS));
			glfwPollEvents();
			inputManager->update();
			handleInput();
			renderTime.reset();
			glEnable(GL_DEPTH_TEST);

			stackBuffer.reset();
			updateSceneControlObjects();

			//Geometry pass
			gBuffer.fill(geometryPassFunc);
			renderTime.update(RenderPass::Geometry);

			//Hacky: Read camera depth from geometry pass and write it into the scene
			scene->forwardScreenInfo(gBuffer.screenInfo);

			scene->lightManager.draw(*scene, &scene->getCamera());
			renderTime.update(RenderPass::Shadow);

			//SSAO pass
			if (ssao != nullptr) {
				ssaoBuffer->fill(*ssao);
				FrameBuffer::resetSize(window->resolution);
				renderTime.update(RenderPass::SSAO);
			}

			//Lighting & forward pass
			stackBuffer.fill(lightingPassFunc);
			renderTime.update(RenderPass::Lighting);

			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			//Post processing
			//Draw single effect if wanted
			if (isolatedEffect != nullptr) {
				PostProcessingEffect* def = effects.front();

				//Save regular rendering settings
				bool onlyEffect = isolatedEffect->getEffectOnly();
				const Texture& buffer = isolatedEffect->getImageBuffer();

				//Draw isolated effect
				isolatedEffect->effectOnly(true);
				isolatedEffect->setImageBuffer(stackBuffer.getTexture(0));
				isolatedEffect->fill();
				
				def->draw();

				//Restore render settings
				isolatedEffect->effectOnly(onlyEffect);
				isolatedEffect->setImageBuffer(buffer);
			}
			//Draw all included post effects
			else {
				//Draw all the post processing effects on top of each other.
				for (auto effect = next(effects.begin()); effect != effects.end(); effect++)
					(**effect).fill();

				//FrameBuffer::resetSize(window->resolution);
				//Draw the last (default) effect to screen.
				effects.front()->draw();
			}

			renderTime.update(RenderPass::PostProcessing);

			//Render GUI overlay on top of final image
			if (gui != nullptr) {
				gui->draw();
				renderTime.update(RenderPass::GUI);
			}
			
			window->swapBuffer();
			Time::update();
		}

		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->quit();

		window->close();
	}

	void DeferredRenderer::draw() {
		glEnable(GL_DEPTH_TEST);

		//Geometry pass
		gBuffer.fill(geometryPassFunc);

		scene->lightManager.drawShadowmaps(*scene, &scene->getCamera());

		//SSAO pass
		if (ssao != nullptr) {
			ssaoBuffer->fill(*ssao);
			FrameBuffer::resetSize(window->resolution);
		}

		const IFrameBuffer* parent = getParentBuffer();
		if (parent != nullptr)
			parent->bind();
		else
			throw "No parent for deferred renderer set\n";

		getParentBuffer()->bind();
		lightingPass();
		scene->drawSkybox();
	}

	void DeferredRenderer::draw(const Camera& camera, const FrameBuffer& buffer) {
		glEnable(GL_DEPTH_TEST);

		//Geometry pass
		gBuffer.fill([this, camera] () { scene->drawDeferred(camera); });

		scene->lightManager.drawShadowmaps(*scene, nullptr);

		//SSAO pass
		if (ssao != nullptr) {
			ssao->setCamera(camera);
			ssaoBuffer->fill(*ssao);
			ssao->updateCamera(scene->getCamera());
		}

		buffer.resetSize();
		buffer.bind();

		//Draw lighting pass and skybox directly into given framebuffer
		lightingPass(camera);
		scene->drawSkybox(camera);
	}

	
	void DeferredRenderer::geometryPass() {
		scene->update();
		scene->drawDeferred();
	}

	void DeferredRenderer::lightingPass() {
		lighting.draw();
	}

	void DeferredRenderer::lightingPass(const Camera& camera) {
		lighting.setCamera(camera);
		lighting.draw();
		lighting.updateCamera(scene->getCamera());
	}


	void DeferredRenderer::forwardPass() {
		glClear(GL_DEPTH_BUFFER_BIT);
		//Copy depth buffer from gBuffer to draw forward 
		//rendered objects 'into' the scene instead of 'on top'
		stackBuffer.copyDepth(gBuffer);

		//Forward pass
		scene->drawForward();
		scene->drawSkybox();
	}

	void DeferredRenderer::handleInput() {
		scene->getCamera().handleInput(*inputManager);
	}

	void DeferredRenderer::addSSAO(SSAO& ssao) {
		this->ssao = &ssao;
		addRequester(*this->ssao);

		ssaoBuffer = new ColorBuffer();
		ssaoBuffer->init(Resolution(window->resolution, ssao.getResolution()),
			ColorType::Single, FilterMode::None, WrapMode::ClampEdge);

		ssaoScreen = new ScreenQuad();
		ssaoScreen->init();
		ssao.init(*ssaoScreen, *ssaoBuffer);
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect) {
		effects.push_back(&effect);
		linkImageBuffer(effect);
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect, WorldMapRequester& requester) {
		effects.push_back(&effect);
		linkImageBuffer(effect);

		addRequester(requester);
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect, list<WorldMapRequester*> requester) {
		effects.push_back(&effect);
		linkImageBuffer(effect);

		for (auto it = requester.begin(); it != requester.end(); it++) {
			WorldMapRequester* req = *it;
			addRequester(*req);
		}
	}

	void DeferredRenderer::addRequester(WorldMapRequester& requester) {
		this->requester.push_back(&requester);
	}

	

	void DeferredRenderer::linkInformation() const {
		//Link world maps to requesting post effects
		map<WorldMaps, const Texture*> worldMaps;
		worldMaps[WorldMaps::Diffuse] = &gBuffer.getDiffuse();
		worldMaps[WorldMaps::PositionRoughness] = &gBuffer.getPositionRoughness();
		worldMaps[WorldMaps::NormalMetallic] = &gBuffer.getNormalMetallic();

		const RenderTexture* emissivity = &gBuffer.getEmissivity();
		if (!emissivity->isEmpty())
			worldMaps[WorldMaps::Emissivity] = emissivity;

		if(ssao != nullptr)
			worldMaps[WorldMaps::SSAO] = &ssaoBuffer->getTexture();

		for (auto it = requester.begin(); it != requester.end(); it++) {
			WorldMapRequester* req = *it;
			req->addWorldInformation(worldMaps);
		}
	}


	void DeferredRenderer::linkImageBuffer(PostProcessingEffect& effect) {
		//Init all post processing effects with two alternating textures
		//Current effect will then always read from one and write to the other
		const Texture& buffer = (effects.size() % 2 == 0) ? 
			stackBuffer.getTexture(0) : 
			stackBuffer.getTexture(1);
		
		effect.setImageBuffer(buffer);
		effect.init(screen, stackBuffer);
	}

	void DeferredRenderer::initDefaultEffect() {
		//Link framebuffer of last added post processing effect to default effect
		const Texture& buffer = (effects.size() % 2 == 0) ?
			stackBuffer.getTexture(1) :
			stackBuffer.getTexture(0);

		effects.front()->setImageBuffer(buffer);
		effects.front()->init(screen, stackBuffer);
	}


	void DeferredRenderer::handleInput(GLFWwindow* window, int key, int scancode, int action, int mode) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
			toggleBuffer(false);
		}
		else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
			toggleBuffer(true);
		}

	}

	void DeferredRenderer::toggleBuffer(bool next) {
		std::vector<const Texture*> buffers = { &effects.front()->getImageBuffer(), 
			&gBuffer.getDiffuse(), &gBuffer.getNormalMetallic() };

		const RenderTexture& emisTex = gBuffer.getEmissivity();
		if (!emisTex.isEmpty())
			buffers.push_back(&emisTex);
		if (ssao != nullptr)
			buffers.push_back(&ssaoBuffer->getTexture());

		//int bufferSize = 4;
		int max = int(buffers.size() + effects.size());
		int i = next ? 1 : -1;
		toggle = abs((toggle + i) % max);

		const Texture* currBuffer = buffers[0];
		if (toggle < buffers.size()) {
			currBuffer = buffers[toggle];

			isolatedEffect = nullptr;
		}
		else {
			currBuffer = &stackBuffer.getTexture(1);

			int index = toggle - buffers.size();
			isolatedEffect = effects[index];
		}

		effects.front()->setImageBuffer(*currBuffer);
		
	}

	const RenderTime& DeferredRenderer::getRenderTime() const {
		return renderTime;
	}
}