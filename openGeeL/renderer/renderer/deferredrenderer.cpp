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
#include "../lighting/lightmanager.h"
#include "../scene.h"
#include "../../interface/guirenderer.h"
#include "deferredrenderer.h"

#define fps 10

using namespace std;

namespace geeL {

	DeferredRenderer::DeferredRenderer(RenderWindow& window, InputManager& inputManager, 
		RenderContext& context, DefaultPostProcess& def, const MaterialFactory& factory)
			: Renderer(window, inputManager, context), frameBuffer1(ColorBuffer()), frameBuffer2(ColorBuffer()),
				gBuffer(GBuffer()), screen(ScreenQuad()), ssao(nullptr),
				deferredShader(new Shader("renderer/shaders/deferredlighting.vert",
					"renderer/shaders/cooktorrancedeferred.frag")), toggle(0), factory(factory) {

		effects.push_back(&def);

		geometryPassFunc = [this]() { this->geometryPass(); };
		lightingPassFunc = [this]() { this->lightingPass(scene->getCamera()); this->forwardPass(); };
	}

	DeferredRenderer::~DeferredRenderer() {
		delete deferredShader;

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
				ColorType::Single, FilterMode::Nearest, WrapMode::Repeat, false);
		}

		deferredShader->use();
		deferredShader->addMap(gBuffer.getPositionDepth(), "gPositionDepth");
		deferredShader->addMap(gBuffer.getNormalMetallic(), "gNormalMet");
		deferredShader->addMap(gBuffer.getDiffuseSpecular(), "gDiffuseSpec");

		invViewLocation = deferredShader->getLocation("inverseView");
		originLocation = deferredShader->getLocation("origin");
		
		if (ssao != nullptr) {
			deferredShader->addMap(ssaoBuffer->getTexture().getID(), "ssao");
			deferredShader->setInteger("useSSAO", 1);
		}

		frameBuffer1.init(unsigned int(window->width), unsigned int(window->height), ColorType::RGBA16);
		frameBuffer2.init(unsigned int(window->width), unsigned int(window->height), ColorType::RGBA16);
		screen.init();
		screen.init();
	}

	void DeferredRenderer::renderInit() {

		deferredShader->use();
		scene->bindSkybox(*deferredShader);
		scene->lightManager.bindShadowmaps(*deferredShader);

		//Init all effects
		bool chooseBuffer = true;
		for (auto effect = effects.begin(); effect != effects.end(); effect++) {
			(*effect)->init(screen, chooseBuffer ? frameBuffer1.info : frameBuffer2.info);
			chooseBuffer = !chooseBuffer;
		}

		//Set color buffer of default effect depending on the amount of added effects
		defaultBuffer = (effects.size() % 2 == 0)
			? frameBuffer2.getTexture().getID()
			: frameBuffer1.getTexture().getID();

		effects.front()->setBuffer(defaultBuffer);

		//Init SSAO (if added)
		if (ssao != nullptr) {
			ssaoScreen = new ScreenQuad();
			ssaoScreen->init();
			ssao->init(*ssaoScreen, ssaoBuffer->info);
		}
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
			
			//Geometry pass
			gBuffer.fill(geometryPassFunc);
			renderTime.update(RenderPass::Geometry);

			//Hacky: Read camera depth from geometry pass and write it into the scene
			scene->forwardScreenInfo(gBuffer.screenInfo);

			glCullFace(GL_BACK);
			scene->lightManager.drawShadowmaps(*scene, &scene->getCamera());
			glCullFace(GL_FRONT);
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
				int counter = 0;
				//Draw all the post processing effects on top of each other. Ping pong style!
				for (auto effect = next(effects.begin()); effect != effects.end(); effect++) {
					if (chooseBuffer)
						frameBuffer2.fill(**effect);
					else
						frameBuffer1.fill(**effect);

					chooseBuffer = !chooseBuffer;
					counter++;
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

		glCullFace(GL_BACK);
		scene->lightManager.drawShadowmaps(*scene, &scene->getCamera());
		glCullFace(GL_FRONT);

		//SSAO pass
		if (ssao != nullptr) {
			ssaoBuffer->fill(*ssao);
			ColorBuffer::resetSize(window->width, window->height);
		}

		//TODO: parent FBO needs to be reset here
		lightingPass(scene->getCamera());

		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
	}

	void DeferredRenderer::draw(const Camera& camera, FrameBufferInformation info) {
		glEnable(GL_DEPTH_TEST);

		//Geometry pass
		gBuffer.fill([this, camera] () { scene->drawDeferred(camera); });

		glCullFace(GL_BACK);
		scene->lightManager.drawShadowmaps(*scene, nullptr);
		glCullFace(GL_FRONT);

		//SSAO pass
		if (ssao != nullptr)
			ssaoBuffer->fill(*ssao);

		FrameBuffer::resetSize(info.width, info.height);
		FrameBuffer::bind(info.fbo);

		//Lighting pass
		lightingPass(camera);

		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
	}

	void DeferredRenderer::geometryPass() {
		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->draw(scene->getCamera());

		scene->update();
		scene->drawDeferred();
	}

	void DeferredRenderer::lightingPass(const Camera& camera) {
		deferredShader->use();
		deferredShader->loadMaps();
		scene->lightManager.bind(camera, *deferredShader, ShaderTransformSpace::View);
		deferredShader->setMat4(invViewLocation, camera.getInverseViewMatrix());
		deferredShader->setVector3(originLocation, camera.GetOriginInViewSpace());
		//deferredShader->setVector3("ambient", scene->lightManager.ambient);
		screen.draw();

		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void DeferredRenderer::forwardPass() {
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
		worldMaps[WorldMaps::DiffuseRoughness] = &gBuffer.getDiffuseSpecular();
		worldMaps[WorldMaps::PositionDepth]    = &gBuffer.getPositionDepth();
		worldMaps[WorldMaps::NormalMetallic]   = &gBuffer.getNormalMetallic();

		for (auto it = requester.begin(); it != requester.end(); it++) {
			WorldMapRequester* req = *it;
			req->addWorldInformation(worldMaps);
		}
	}


	void DeferredRenderer::linkImageBuffer(PostProcessingEffect& effect) const {

		//Init all post processing effects with two alternating framebuffers
		//Current effect will then always read from one and write to the other
		if (effects.size() % 2 == 0)
			effect.setBuffer(frameBuffer1);
		else
			effect.setBuffer(frameBuffer2);
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
					currBuffer = gBuffer.getDiffuseSpecular().getID();
					break;
				case 2:
					currBuffer = gBuffer.getNormalMetallic().getID();
					break;
				case 3:
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