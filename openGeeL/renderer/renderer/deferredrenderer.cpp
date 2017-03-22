#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <map>
#include <cmath>
#include <gtc/matrix_transform.hpp>
#include "../shader/sceneshader.h"
#include "../postprocessing/drawdefault.h"
#include "../texturing/simpletexture.h"
#include "../window.h"
#include "../scripting/scenecontrolobject.h"
#include "../inputmanager.h"
#include "../postprocessing/ssao.h"
#include "../postprocessing/postprocessing.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"
#include "../shader/shadermanager.h"
#include "../transformation/transform.h"
#include "../scene.h"
#include "../../interface/guirenderer.h"
#include "deferredrenderer.h"

#define fps 10

using namespace std;

namespace geeL {

	DeferredRenderer::DeferredRenderer(RenderWindow& window, InputManager& inputManager, 
		RenderContext& context, DefaultPostProcess& def)
			: DeferredRenderer(window, inputManager, context, def, nullptr) {}

	DeferredRenderer::DeferredRenderer(RenderWindow& window, InputManager& inputManager, 
		RenderContext& context, DefaultPostProcess& def, SSAO* ssao, float ssaoResolution)
			: Renderer(window, inputManager, context), ssao(ssao), frameBuffer1(FrameBuffer()), frameBuffer2(FrameBuffer()),
				gBuffer(GBuffer()), screen(ScreenQuad()), ssaoResolution(ssaoResolution),
				deferredShader(new Shader("renderer/shaders/deferredlighting.vert",
					"renderer/shaders/cooktorrancedeferred.frag")), toggle(0) {

		effects.push_back(&def);

		if(ssao != nullptr)
			addRequester(*ssao);
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
			ssaoBuffer = new FrameBuffer();
			ssaoBuffer->init(window->width * ssaoResolution, window->height * ssaoResolution, 
				1, ColorType::Single, FilterMode::Nearest, WrapMode::Repeat, false);
		}

		deferredShader->use();
		deferredShader->mapOffset = 1;
		deferredShader->addMap(gBuffer.positionDepth, "gPositionDepth");
		deferredShader->addMap(gBuffer.normalMet, "gNormalMet");
		deferredShader->addMap(gBuffer.diffuseSpec, "gDiffuseSpec");
		
		if (ssao != nullptr) {
			deferredShader->addMap(ssaoBuffer->getColorID(), "ssao");
			deferredShader->setInteger("useSSAO", 1);
		}

		//frameBuffer1.init(window->width, window->height, 2, { RGBA16, Single });
		//frameBuffer2.init(window->width, window->height, 2, { RGBA16, Single });
		frameBuffer1.init(window->width, window->height, 1, ColorType::RGBA16);
		frameBuffer2.init(window->width, window->height, 1, ColorType::RGBA16);
		screen.init();
		screen.init();
	}

	void DeferredRenderer::renderInit() {

		scene->bindSkybox(*deferredShader);
		deferredShader->bindMaps();

		//Init all effects
		bool chooseBuffer = true;
		for (auto effect = effects.begin(); effect != effects.end(); effect++) {
			(*effect)->init(screen, chooseBuffer ? frameBuffer1.info : frameBuffer2.info);
			chooseBuffer = !chooseBuffer;
		}

		//Set color buffer of default effect depending on the amount of added effects
		defaultBuffer = (effects.size() % 2 == 0)
			? frameBuffer2.getColorID()
			: frameBuffer1.getColorID();

		effects.front()->setBuffer(defaultBuffer);

		//Init SSAO (if added)
		if (ssao != nullptr) {
			ssaoScreen = new ScreenQuad();
			ssaoScreen->init();
			ssao->init(*ssaoScreen, ssaoBuffer->info);
		}

		shaderManager->staticDeferredBind(*scene, *deferredShader);
		shaderManager->staticBind(*scene);
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
			gBuffer.fill(*this);
			renderTime.update(RenderPass::Geometry);

			//Hacky: Read camera depth from geometry pass and write it into the scene
			scene->forwardScreenInfo(gBuffer.screenInfo);
			scene->lightManager.drawShadowmaps(*scene);
			renderTime.update(RenderPass::Shadow);

			//SSAO pass
			if (ssao != nullptr) {
				ssaoBuffer->fill(*ssao);
				FrameBuffer::resetSize(window->width, window->height);
				renderTime.update(RenderPass::SSAO);
			}

			//Lighting & forward pass
			frameBuffer1.fill(*this);
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
				isolatedEffect->setBuffer(frameBuffer1.getColorID());
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
		if (geometryPass) {
			for (size_t i = 0; i < objects.size(); i++)
				objects[i]->draw(scene->camera);

			scene->update();
			shaderManager->bindCamera(*scene);
			scene->drawDeferred();
		}
		else {
			//glClear(GL_COLOR_BUFFER_BIT);
			//Lighting pass
			deferredShader->use();
			deferredShader->loadMaps();
			scene->lightManager.deferredBind(*scene, *deferredShader);
			deferredShader->setMat4("inverseView", glm::inverse(scene->camera.getViewMatrix()));
			deferredShader->setVector3("origin", scene->GetOriginInViewSpace());
			deferredShader->setVector3("ambient", scene->lightManager.ambient);
			screen.draw();

			glClear(GL_DEPTH_BUFFER_BIT);
			//Copy depth buffer from gBuffer to draw forward 
			//rendered objects 'into' the scene instead of 'on top'
			frameBuffer1.copyDepth(gBuffer.fbo);

			//Forward pass
			shaderManager->dynamicBind(*scene);
			scene->drawForward();
			scene->drawSkybox();
		}

		geometryPass = !geometryPass;
	}

	void DeferredRenderer::handleInput() {
		scene->camera.handleInput(*inputManager);
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect) {
		effects.push_back(&effect);
		linkImageBuffer(effect);
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect, WorldInformationRequester& requester) {
		effects.push_back(&effect);
		linkImageBuffer(effect);

		addRequester(requester);
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect, list<WorldInformationRequester*> requester) {
		effects.push_back(&effect);
		linkImageBuffer(effect);

		for (auto it = requester.begin(); it != requester.end(); it++) {
			WorldInformationRequester* req = *it;
			addRequester(*req);
		}
	}

	void DeferredRenderer::addRequester(WorldInformationRequester& requester) {
		this->requester.push_back(&requester);
	}

	void DeferredRenderer::linkInformation() const {

		map<WorldMaps, unsigned int> worldMaps;
		worldMaps[WorldMaps::DiffuseRoughness] = gBuffer.diffuseSpec;
		worldMaps[WorldMaps::PositionDepth]    = gBuffer.positionDepth;
		worldMaps[WorldMaps::NormalMetallic]   = gBuffer.normalMet;

		//ISSUE: If scene camera changes, everything is fucked
		map<WorldMatrices, const mat4*> worldMatrices;
		worldMatrices[WorldMatrices::View]        = &scene->camera.getViewMatrix();
		worldMatrices[WorldMatrices::Projection]  = &scene->camera.getProjectionMatrix();
		worldMatrices[WorldMatrices::InverseView] = &scene->camera.getInverseViewMatrix();

		map<WorldVectors, const vec3*> worldVectors;
		worldVectors[WorldVectors::CameraPosition]  = &scene->camera.getPosition();
		worldVectors[WorldVectors::CameraDirection] = &scene->camera.getDirection();
		worldVectors[WorldVectors::OriginView]      = &scene->GetOriginInViewSpace();

		for (auto it = requester.begin(); it != requester.end(); it++) {
			WorldInformationRequester* req = *it;
			req->addWorldInformation(worldMaps, worldMatrices, worldVectors);
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
		int max = bufferSize + effects.size();
		int i = next ? 1 : -1;
		toggle = abs((toggle + i) % max);

		unsigned int currBuffer = 1;
		if (toggle < bufferSize) {
			switch (toggle) {
				case 0:
					currBuffer = defaultBuffer;
					break;
				case 1:
					currBuffer = gBuffer.diffuseSpec;
					break;
				case 2:
					currBuffer = gBuffer.normalMet;
					break;
				case 3:
					currBuffer = ssaoBuffer->getColorID();
					break;
			}

			isolatedEffect = nullptr;
		}
		else {
			currBuffer = frameBuffer2.getColorID();

			int index = toggle - bufferSize;
			isolatedEffect = effects[index];
		}

		effects.front()->setBuffer(currBuffer);
	}

	const RenderTime& DeferredRenderer::getRenderTime() const {
		return renderTime;
	}
}