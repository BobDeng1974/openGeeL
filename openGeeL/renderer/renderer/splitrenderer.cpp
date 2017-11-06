#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>

#include "utility/rendertime.h"
#include "splitrenderer.h"
#include "window.h"
#include "utility/viewport.h"
#include "utility/glguards.h"
#include "inputmanager.h"


namespace geeL {

	SplitRenderer::SplitRenderer(RenderWindow& window, Input& inputManager, RenderContext& context)
		: Renderer(window, inputManager, context) {
	
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			std::cout << "Failed to initialize GLEW" << std::endl;
		}

		Viewport::set(0, 0, window.getWidth(), window.getHeight());
		DepthGuard::enable(true);
	}


	void SplitRenderer::run() {
		glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw();
		window->swapBuffer();

		handleInput();
	}

	void SplitRenderer::draw() {
		for (size_t i = 0; i < renderers.size(); i++) {
			pair<Renderer*, RenderViewport>pair = renderers[i];
			Renderer* renderer = pair.first;
			RenderViewport view = pair.second;

			Viewport::set(view.x * window->getWidth(), view.y * window->getHeight(),
				view.width * window->getWidth(), view.height * window->getHeight());
			renderer->draw();
		}
	}

	void SplitRenderer::handleInput() {
		float mouseX = input.getMouseXNorm();
		float mouseY = input.getMouseYNorm();
		
		for (size_t i = renderers.size(); i > 0 ; i--) {
			pair<Renderer*, RenderViewport>pair = renderers[i - 1];
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