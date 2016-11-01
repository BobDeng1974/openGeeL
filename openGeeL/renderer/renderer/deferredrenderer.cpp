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
#include "../postprocessing/postprocessing.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"
#include "../shader/shadermanager.h"
#include "../transformation/transform.h"
#include "../scene.h"

#define fps 10

namespace geeL {

	DeferredRenderer::DeferredRenderer(RenderWindow* window, InputManager* inputManager)
		:
		Renderer(window, inputManager), effect(nullptr),
		frameBuffer(FrameBuffer()), gBuffer(GBuffer()), screen(ScreenQuad(window->width, window->height)),
		deferredShader(new Shader("renderer/shaders/deferredlighting.vert",
			"renderer/shaders/deferredlighting.frag")) {

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
	}

	DeferredRenderer::~DeferredRenderer() {
		delete deferredShader;
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

		deferredShader->use();
		deferredShader->mapOffset = 1;
		deferredShader->addMap(gBuffer.positionDepth, "gPositionDepth");
		deferredShader->addMap(gBuffer.normal, "gNormal");
		deferredShader->addMap(gBuffer.diffuseSpec, "gDiffuseSpec");
		//deferredShader->bindMaps();

		frameBuffer.init(window->width, window->height);
		screen.init();
	}

	void DeferredRenderer::render() {

		DefaultPostProcess defaultEffect = DefaultPostProcess();
		defaultEffect.setScreen(screen);
		shaderManager->staticDeferredBind(*scene, *deferredShader);
		shaderManager->staticBind(*scene);

		while (!window->shouldClose()) {
			int currFPS = ceil(Time::deltaTime * 1000.f);
			std::this_thread::sleep_for(std::chrono::milliseconds(fps - currFPS));
			glfwPollEvents();
			inputManager->update();
			handleInput();

			glEnable(GL_DEPTH_TEST);
			scene->lightManager.drawShadowmaps(*scene);

			//Geometry path
			gBuffer.fill(*this);
			
			//Lighting path & forward pass
			frameBuffer.fill(*this);

			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			if (effect != nullptr) {
				effect->setBuffer(frameBuffer.color);
				effect->draw();
			}
			//Default post processing(Gamma correction & tone mapping)
			else {
				defaultEffect.setBuffer(frameBuffer.color);
				defaultEffect.draw();
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
			screen.draw();

			glClear(GL_DEPTH_BUFFER_BIT);
			//Copy depth buffer from gBuffer to draw forward 
			//rendered objects 'into' the scene instead of 'on top'
			frameBuffer.copyDepth(gBuffer.fbo);

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

	void DeferredRenderer::setEffect(PostProcessingEffect& effect) {
		this->effect = &effect;
		this->effect->setScreen(screen);
	}

}