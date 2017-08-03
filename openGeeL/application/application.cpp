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

	Application::Application(RenderWindow& window, InputManager& inputManager, RenderThread& renderThread)
		: window(window), inputManager(inputManager), renderer(renderThread.getRenderer()) {

		auto exit = [&window](int key, int scancode, int action, int mode) { 
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
				glfwSetWindowShouldClose(window.glWindow, GL_TRUE);
		};

		inputManager.addCallback(exit);
		inputManager.init(&window);

		addThread(renderThread);
	}


	mutex inputLock;
	bool close = false;

	void Application::run() {
		renderer.linkInformation();
		glfwMakeContextCurrent(0);
		
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

		joinThreads();

		window.close();
	}

	void Application::addThread(ContinuousThread& thread) {
		tempThreads.push_back(&thread);
	}

	bool Application::closing() {
		return close;
	}

	const ContinuousThread* const Application::getThread(ThreadID id) {
		auto it = threads.find(id);
		if (it != threads.end())
			return it->second.first;

		return nullptr;
	}

	const ContinuousThread* const Application::getCurrentThread() {
		return getThread(this_thread::get_id());
	}

	void Application::initThreads() {
		for (auto it(tempThreads.begin()); it != tempThreads.end(); it++) {
			ContinuousThread& thread = **it;

			std::thread t = std::move(thread.start());
			threads[t.get_id()] = pair<ContinuousThread*, std::thread>(&thread, std::move(t));
		}

		tempThreads.clear();
	}

	void Application::joinThreads() {
		for (auto it(threads.begin()); it != threads.end(); it++) {
			std::thread* thread = &it->second.second;

			if(thread->joinable())
				thread->join();
		}
	}

}