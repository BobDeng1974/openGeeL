#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>

#include "splitrenderer.h"
#include "../window.h"
#include "../renderobject.h"
#include "../inputmanager.h"
#include "../shader/shadermanager.h"

#define fps 10

namespace geeL {

	SplitRenderer::SplitRenderer(RenderWindow* window, InputManager* inputManager, ShaderManager* shaderManger)
		: Renderer(window, inputManager, shaderManger) {
	
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			std::cout << "Failed to initialize GLEW" << std::endl;
		}

		glViewport(0, 0, window->width, window->height);
		glEnable(GL_DEPTH_TEST);
	}


	void SplitRenderer::init() {
		for (size_t i = 0; i < renderers.size(); i++) {
			pair<Renderer*, RenderViewport>pair = renderers[i];
			Renderer* renderer = pair.first;

			renderer->init();
		}

		inputManager->init(window);
	}

	void SplitRenderer::render() {
		GLfloat deltaTime = 0.f;
		GLfloat lastFrame = 0.f;
		GLfloat currentFrame = 0.f;
		
		while (!window->shouldClose()) {

			int currFPS = ceil(deltaTime * 1000);
			std::this_thread::sleep_for(std::chrono::milliseconds(fps - currFPS));

			lastFrame = glfwGetTime();

			glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
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

	void SplitRenderer::renderFrame() {
		for (size_t i = 0; i < renderers.size(); i++) {
			pair<Renderer*, RenderViewport>pair = renderers[i];
			Renderer* renderer = pair.first;
			RenderViewport view = pair.second;

			glViewport(view.x * window->width, view.y * window->height, view.width * window->width, view.height * window->height);
			renderer->renderFrame();
		}
	}

	void SplitRenderer::handleInput() {
		float mouseX = inputManager->getMouseXNorm();
		float mouseY = inputManager->getMouseYNorm();
		
		for (size_t i = 0; i < renderers.size(); i++) {
			pair<Renderer*, RenderViewport>pair = renderers[i];
			RenderViewport view = pair.second;
			
			float minX = view.x;
			float maxX = (view.x + view.width);
			float maxY = -view.y + 1.f;
			float minY = -(view.y + view.height) + 1.f;

			if (minX <= mouseX && maxX >= mouseX && minY <= mouseY && maxY >= mouseY) {
				Renderer* renderer = pair.first;
				renderer->handleInput();
				break;
			}
		}
	}

	void SplitRenderer::addRenderer(Renderer* renderer, RenderViewport view) {
		if (renderer != nullptr) {
			pair<Renderer*, RenderViewport>pair(renderer, view);
			renderers.push_back(pair);
		}

	}
	

}