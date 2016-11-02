#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>
#include <gtc/matrix_transform.hpp>
#include "../utility/rendertime.h"
#include "../shader/shader.h"
#include "../postprocessing/drawdefault.h"
#include "deferredrenderer.h"
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

#define fps 10

using namespace std;

namespace geeL {

	DeferredRenderer::DeferredRenderer(RenderWindow* window, InputManager* inputManager) 
		: DeferredRenderer(window, inputManager, nullptr) {}

	DeferredRenderer::DeferredRenderer(RenderWindow* window, InputManager* inputManager, SSAO* ssao) 
		:
		Renderer(window, inputManager), ssao(ssao), frameBuffer1(FrameBuffer()), frameBuffer2(FrameBuffer()),
			gBuffer(GBuffer()), screen(ScreenQuad(window->width, window->height)),
			deferredShader(new Shader("renderer/shaders/deferredlighting.vert",
				"renderer/shaders/deferredlighting.frag")) {

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glFrontFace(GL_CW);

			//Default post processing with tone mapping and gamma correction
			DefaultPostProcess* defaultEffect = new DefaultPostProcess();
			defaultEffect->setScreen(screen);
			effects.push_back(defaultEffect);
	}

	DeferredRenderer::~DeferredRenderer() {
		delete effects.front();
		delete deferredShader;

		if (ssaoBuffer != nullptr)
			delete ssaoBuffer;
	}


	void exitCallbackkk(GLFWwindow* window, int key, int scancode, int action, int mode) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	void DeferredRenderer::init() {
		inputManager->addCallback(exitCallbackkk);
		inputManager->init(window);

		gBuffer.init(window->width, window->height);

		if (ssao != nullptr) {
			ssaoBuffer = new FrameBuffer();
			ssaoBuffer->init(window->width, window->height, false, Single, GL_NEAREST);
		}

		deferredShader->use();
		deferredShader->mapOffset = 1;
		deferredShader->addMap(gBuffer.positionDepth, "gPositionDepth");
		deferredShader->addMap(gBuffer.normal, "gNormal");
		deferredShader->addMap(gBuffer.diffuseSpec, "gDiffuseSpec");
		
		if (ssao != nullptr)
			deferredShader->addMap(ssaoBuffer->color, "ssao");

		frameBuffer1.init(window->width, window->height);
		frameBuffer2.init(window->width, window->height);
		screen.init();
	}

	void DeferredRenderer::render() {

		//Init all post processing effects with two alternating framebuffers
		//Current effect will then always read from one and write to one
		bool chooseBuffer = true;
		for (list<PostProcessingEffect*>::reverse_iterator it = effects.rbegin(); 
			it != effects.rend(); it++) {

			(*it)->setBuffer(chooseBuffer ? frameBuffer1.color : frameBuffer2.color);
			(*it)->setParentFBO(chooseBuffer ? frameBuffer2.fbo : frameBuffer1.fbo);

			chooseBuffer = !chooseBuffer;
		}

		//Init SSAO (if added)
		if (ssao != nullptr) {
			std::list<unsigned int> ssaoMaps = { gBuffer.positionDepth, gBuffer.normal };
			ssao->setBuffer(ssaoMaps);

			ssao->setScreen(screen);
			ssao->setParentFBO(ssaoBuffer->fbo);
		}

		shaderManager->staticDeferredBind(*scene, *deferredShader);
		shaderManager->staticBind(*scene);

		//Render loop
		while (!window->shouldClose()) {
			int currFPS = ceil(Time::deltaTime * 1000.f);
			std::this_thread::sleep_for(std::chrono::milliseconds(fps - currFPS));
			glfwPollEvents();
			inputManager->update();
			handleInput();

			glEnable(GL_DEPTH_TEST);
			scene->lightManager.drawShadowmaps(*scene);

			//Geometry pass
			gBuffer.fill(*this);

			//SSAO pass
			if (ssao != nullptr) {
				ssaoBuffer->fill(*ssao);
				//effects.front()->setBuffer(ssaoBuffer->color);
			}

			//Lighting & forward pass
			frameBuffer1.fill(*this);

			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			//Post processing
			chooseBuffer = true;
			//Draw all the post processing effects on top of each other. Ping pong style!
			for (list<PostProcessingEffect*>::reverse_iterator it = effects.rbegin(); 
				it != prev(effects.rend()); it++) {
				
				if (chooseBuffer)
					frameBuffer2.fill(**it);
				else
					frameBuffer1.fill(**it);

				chooseBuffer = !chooseBuffer;
			}
			//Draw the last (default) effect to screen.
			effects.front()->draw();

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
		effect.setScreen(screen);
		effects.push_back(&effect);
	}

}