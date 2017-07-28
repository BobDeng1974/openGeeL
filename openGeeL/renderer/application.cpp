#include <glfw3.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <mutex>
#include <iostream>
#include "utility\rendertime.h"
#include "renderer.h"
#include "window.h"
#include "inputmanager.h"
#include "application.h"
 
using namespace std;

namespace geeL {

	Application::Application(RenderWindow& window, InputManager& inputManager, Renderer& renderer)
		: window(window), inputManager(inputManager), renderer(renderer) {

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