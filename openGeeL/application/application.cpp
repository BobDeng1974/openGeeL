#include <glfw3.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <mutex>
#include <functional>
#include <iostream>
#include "../renderer/utility/rendertime.h"
#include "../renderer/renderer.h"
#include "../renderer/window.h"
#include "../renderer/inputmanager.h"
#include "application.h"
 
using namespace std;

namespace geeL {

	Application::Application(RenderWindow& window, InputManager& inputManager, Renderer& renderer)
		: window(window), inputManager(inputManager), renderer(renderer) {

		auto exit = [this, &window](int key, int scancode, int action, int mode) { 
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
				glfwSetWindowShouldClose(window.glWindow, GL_TRUE);
		};

		inputManager.addCallback(exit);
		inputManager.init(&window);
	}

	mutex inputLock;
	bool close = false;

	void Application::run() {
		glfwMakeContextCurrent(0);
		
		thread renderThread([this]() { renderer.render(); });
		
		Time& time = Time();
		Time& inner = Time();

		long FPS = 10;

		while (!window.shouldClose()) {
			inner.reset();

			inputLock.lock();
			glfwPollEvents();
			inputManager.update();
			inputLock.unlock();

			inner.update();
			long currFPS = std::max(0L, FPS - inner.deltaTime());
			this_thread::sleep_for(chrono::milliseconds(currFPS));

			time.update();
		}

		close = true;

		renderThread.join();
		window.close();
	}

	bool Application::closing() {
		return close;
	}

}