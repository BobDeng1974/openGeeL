#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>

#include "../utility/rendertime.h"
#include "../shader/shader.h"
#include "../postprocessing/drawdefault.h"
#include "postrenderer.h"
#include "../window.h"
#include "../scripting/scenecontrolobject.h"
#include "../inputmanager.h"
#include "../postprocessing/postprocessing.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"
#include "../shader/shadermanager.h"
#include "../scene.h"

#define fps 10

namespace geeL {

	PostProcessingRenderer::PostProcessingRenderer(RenderWindow* window, InputManager* inputManager)
		: 
		Renderer(window, inputManager), effect(nullptr), 
		frameBuffer(FrameBuffer()), screen(ScreenQuad(window->width, window->height)) {

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
	}
	
	void exitCallbackk(GLFWwindow* window, int key, int scancode, int action, int mode) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	void PostProcessingRenderer::init() {
		inputManager->addCallback(exitCallbackk);
		inputManager->init(window);
		
		frameBuffer.init(window->width, window->height);
		screen.init();
	}

	void PostProcessingRenderer::render() {

		DefaultPostProcess defaultEffect = DefaultPostProcess();
		defaultEffect.setScreen(screen);
		shaderManager->staticBind(*scene);

		while (!window->shouldClose()) {
			int currFPS = ceil(Time::deltaTime * 1000.f);
			std::this_thread::sleep_for(std::chrono::milliseconds(fps - currFPS));
			glfwPollEvents();
			inputManager->update();
			handleInput();

			glEnable(GL_DEPTH_TEST);
			scene->lightManager.drawShadowmaps(*scene);
			frameBuffer.fill(*this);
			
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			if (effect != nullptr) {
				effect->setBuffer(frameBuffer.getColorID());
				effect->draw();
			}
			//Default rendering
			else {
				defaultEffect.setBuffer(frameBuffer.getColorID());
				defaultEffect.draw();
			}
			
			window->swapBuffer();
			Time::update();
		}

		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->quit();

		window->close();
	}

	void PostProcessingRenderer::draw() {
		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->draw(scene->camera);

		shaderManager->dynamicBind(*scene);
		scene->draw();
	}

	void PostProcessingRenderer::handleInput() {
		scene->camera.handleInput(*inputManager);
	}

	void PostProcessingRenderer::setEffect(PostProcessingEffect& effect) {
		this->effect = &effect;
		this->effect->setScreen(screen);
	}

}