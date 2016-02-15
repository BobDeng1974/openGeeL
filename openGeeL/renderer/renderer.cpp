#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>

#include "renderer.h"
#include "window.h"
#include "renderobject.h"
#include "inputmanager.h"

#define fps 10

namespace geeL {


	Renderer::Renderer(RenderWindow* window, InputManager* inputManager)
		: window(window), inputManager(inputManager) {}

	Renderer::~Renderer() {
		//delete name;
	}

	void exitCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}			

	void Renderer::render() {
		
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			std::cout << "Failed to initialize GLEW" << std::endl;
		}

		glViewport(0, 0, window->width, window->height);
		glEnable(GL_DEPTH_TEST);
		
		for (size_t i = 0; i < objects.size(); i++) {
			objects[i]->window = window;
			objects[i]->inputManager = inputManager;
			objects[i]->init();
		}
		
		inputManager->addCallback(exitCallback);
		inputManager->init(window);

		GLfloat deltaTime = 0.f;
		GLfloat lastFrame = 0.f;
		GLfloat currentFrame = 0.f;

		while (!window->shouldClose()) {
			
			int currFPS = ceil(deltaTime * 1000);
			std::this_thread::sleep_for(std::chrono::milliseconds(fps - currFPS));
			
			lastFrame = glfwGetTime();

			renderFrame();
			handleInput();
			
			currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
		}

		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->quit();

		window->close();
	}

	void Renderer::renderFrame() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->update();

		window->swapBuffer();
	}

	void Renderer::handleInput() {
		glfwPollEvents();
		inputManager->update();
	}

	void Renderer::addObject(RenderObject* obj) {
		objects.push_back(obj);
	}

}