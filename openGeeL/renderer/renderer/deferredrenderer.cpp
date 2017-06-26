#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <map>
#include "../shader/sceneshader.h"
#include "../postprocessing/drawdefault.h"
#include "../texturing/imagetexture.h"
#include "../window.h"
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
		RenderContext& context, DefaultPostProcess& def, const MaterialFactory& factory)
			: Renderer(window, inputManager, context), frameBuffer1(ColorBuffer()), frameBuffer2(ColorBuffer()),
				gBuffer(GBuffer()), screen(ScreenQuad()), ssao(nullptr), lighting(lighting),
				toggle(0), factory(factory) {

		effects.push_back(&def);
		addRequester(lighting);

		geometryPassFunc = [this]() { this->geometryPass(); };
		lightingPassFunc = [this]() { this->lightingPass(); this->forwardPass(); };
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

		gBuffer.init(window->width, window->height);

		if (ssao != nullptr) {
			ssaoBuffer = new ColorBuffer();
			ssaoBuffer->init(unsigned int(window->width * ssaoResolution), unsigned int(window->height * ssaoResolution),
				ColorType::Single, FilterMode::None, WrapMode::Repeat, false);
		}

		frameBuffer1.init(unsigned int(window->width), unsigned int(window->height), ColorType::RGBA16);
		frameBuffer2.init(unsigned int(window->width), unsigned int(window->height), ColorType::RGBA16);
		screen.init();
		screen.init();
	}

	void DeferredRenderer::renderInit() {

		//Init SSAO (if added)
		if (ssao != nullptr) {
			ssaoScreen = new ScreenQuad();
			ssaoScreen->init();
			ssao->init(*ssaoScreen, *ssaoBuffer);
		}
		
		lighting.init(screen, frameBuffer1);
		scene->init();
		
		//Init all effects
		bool chooseBuffer = true;
		for (auto effect = effects.begin(); effect != effects.end(); effect++) {
			(*effect)->init(screen, chooseBuffer ? frameBuffer1 : frameBuffer2);
			chooseBuffer = !chooseBuffer;
		}

		//Set color buffer of default effect depending on the amount of added effects
		defaultBuffer = (effects.size() % 2 == 0)
			? frameBuffer2.getTexture().getID()
			: frameBuffer1.getTexture().getID();

		effects.front()->setBuffer(defaultBuffer);
	}

	void DeferredRenderer::render() {
		renderInit();

		//Render loop
		while (!window->shouldClose()) {
			int currFPS = (int)ceil(Time::deltaTime * 1000.f);
			this_thread::sleep_for(chrono::milliseconds(fps - currFPS));
			glfwPollEvents();
			inputManager->update();
			handleInput();
			renderTime.reset();
			glEnable(GL_DEPTH_TEST);

			updateSceneControlObjects();

			//Geometry pass
			gBuffer.fill(geometryPassFunc);
			renderTime.update(RenderPass::Geometry);

			//Hacky: Read camera depth from geometry pass and write it into the scene
			scene->forwardScreenInfo(gBuffer.screenInfo);

			scene->lightManager.drawShadowmaps(*scene, &scene->getCamera());
			renderTime.update(RenderPass::Shadow);

			//SSAO pass
			if (ssao != nullptr) {
				ssaoBuffer->fill(*ssao);
				ColorBuffer::resetSize(window->width, window->height);
				renderTime.update(RenderPass::SSAO);
			}

			//Lighting & forward pass
			frameBuffer1.fill(lightingPassFunc);
			renderTime.update(RenderPass::Lighting);

			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			//Post processing
			//Draw single effect if wanted
			if (isolatedEffect != nullptr) {
				PostProcessingEffect* def = effects.front();

				//Save regular rendering settings
				bool onlyEffect = isolatedEffect->getEffectOnly();
				unsigned int buffer = isolatedEffect->getBuffer();

				//Draw isolated effect
				isolatedEffect->effectOnly(true);
				isolatedEffect->setBuffer(frameBuffer1.getTexture().getID());
				frameBuffer2.fill(*isolatedEffect);

				def->draw();

				//Restore render settings
				isolatedEffect->effectOnly(onlyEffect);
				isolatedEffect->setBuffer(buffer);
			}
			//Draw all included post effects
			else {
				bool chooseBuffer = true;
				//Draw all the post processing effects on top of each other. Ping pong style!
				for (auto effect = next(effects.begin()); effect != effects.end(); effect++) {
					ColorBuffer& currBuffer = chooseBuffer ? frameBuffer2 : frameBuffer1;
					currBuffer.fill(**effect);

					chooseBuffer = !chooseBuffer;
				}

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
			FrameBuffer::resetSize(window->width, window->height);
		}

		FrameBuffer::bind(getParentFBO());
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

		FrameBuffer::resetSize(buffer.getWidth(), buffer.getHeight());
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
		frameBuffer1.copyDepth(gBuffer);

		//Forward pass
		scene->drawForward();
		scene->drawSkybox();
	}

	void DeferredRenderer::handleInput() {
		scene->getCamera().handleInput(*inputManager);
	}

	void DeferredRenderer::addSSAO(SSAO& ssao, float ssaoResolution) {
		this->ssao = &ssao;
		this->ssaoResolution = ssaoResolution;

		addRequester(*this->ssao);
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

		map<WorldMaps, const Texture*> worldMaps;
		worldMaps[WorldMaps::Diffuse] = &gBuffer.getDiffuse();
		worldMaps[WorldMaps::PositionRoughness] = &gBuffer.getPositionRoughness();
		worldMaps[WorldMaps::NormalMetallic] = &gBuffer.getNormalMetallic();


		if(ssao != nullptr)
			worldMaps[WorldMaps::SSAO] = &ssaoBuffer->getTexture();

		for (auto it = requester.begin(); it != requester.end(); it++) {
			WorldMapRequester* req = *it;
			req->addWorldInformation(worldMaps);
		}
	}


	void DeferredRenderer::linkImageBuffer(PostProcessingEffect& effect) const {

		//Init all post processing effects with two alternating framebuffers
		//Current effect will then always read from one and write to the other
		const ColorBuffer& buffer = (effects.size() % 2 == 0) ? frameBuffer1 : frameBuffer2;
		effect.setBuffer(buffer);
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
		int bufferSize = 4;
		int max = int(bufferSize + effects.size());
		int i = next ? 1 : -1;
		toggle = abs((toggle + i) % max);

		unsigned int currBuffer = 1;
		if (toggle < bufferSize) {
			switch (toggle) {
				case 0:
					currBuffer = defaultBuffer;
					break;
				case 1:
					currBuffer = gBuffer.getDiffuse().getID();
					break;
				case 2:
					currBuffer = gBuffer.getNormalMetallic().getID();
					break;
				case 3:
					if(ssao != nullptr)
						currBuffer = ssaoBuffer->getTexture().getID();
					break;
			}

			isolatedEffect = nullptr;
		}
		else {
			currBuffer = frameBuffer2.getTexture().getID();

			int index = toggle - bufferSize;
			isolatedEffect = effects[index];
		}

		effects.front()->setBuffer(currBuffer);
	}

	const RenderTime& DeferredRenderer::getRenderTime() const {
		return renderTime;
	}
}