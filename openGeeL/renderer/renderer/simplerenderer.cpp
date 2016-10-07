#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>

#include "simplerenderer.h"
#include "../window.h"
#include "../renderobject.h"
#include "../inputmanager.h"
#include "../cameras/camera.h"

#define fps 10

namespace geeL {


	SimpleRenderer::SimpleRenderer(RenderWindow* window, InputManager* inputManager)
		: Renderer(window, inputManager) {

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			std::cout << "Failed to initialize GLEW" << std::endl;
		}

		glViewport(0, 0, window->width, window->height);
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
	}


	void exitCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	void SimpleRenderer::init() {
		inputManager->addCallback(exitCallback);
		inputManager->init(window);
	}

	void SimpleRenderer::render() {

		GLfloat deltaTime = 0.f;
		GLfloat lastFrame = 0.f;
		GLfloat currentFrame = 0.f;

		while (!window->shouldClose()) {
			int currFPS = ceil(deltaTime * 1000);
			std::this_thread::sleep_for(std::chrono::milliseconds(fps - currFPS));

			lastFrame = glfwGetTime();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderFrame();
			window->swapBuffer();

			glfwPollEvents();
			inputManager->update();
			handleInput();

			currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
		}

		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->quit();

		window->close();
	}

	void SimpleRenderer::renderFrame() {
		currentCamera->update();

		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->draw(*currentCamera);
	}

	void SimpleRenderer::handleInput() {
		currentCamera->handleInput(*inputManager);
	}

	void SimpleRenderer::setCamera(Camera* camera) {
		if (camera != nullptr)
			currentCamera = camera;
	}
	

}