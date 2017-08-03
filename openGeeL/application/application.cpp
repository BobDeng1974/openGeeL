#include <glfw3.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <mutex>
#include <functional>
#include <iostream>
#include "utility/rendertime.h"
#include "renderer.h"
#include "window.h"
#include "inputmanager.h"
#include "threading.h"
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
		renderer.linkInformation();
		glfwMakeContextCurrent(0);
		
		thread renderThread([this]() { renderer.render(); });
		initThreads();

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
		}

		close = true;

		renderThread.join();
		joinThreads();

		window.close();
	}

	void Application::addThread(ContinuousThread& thread) {
		tempThreads.push_back(&thread);
	}

	bool Application::closing() {
		return close;
	}

	void Application::initThreads() {
		for (auto it(tempThreads.begin()); it != tempThreads.end(); it++) {
			ContinuousThread& thread = **it;

			threads.push_back(std::move(thread.start()));
		}

		tempThreads.clear();
	}

	void Application::joinThreads() {
		for (auto it(threads.begin()); it != threads.end(); it++) {
			std::thread& thread = *it;

			if(thread.joinable())
				thread.join();
		}
	}

}