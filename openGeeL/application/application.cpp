#include <glfw3.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <mutex>
#include <functional>
#include <iostream>
#include "utility/rendertime.h"
#include "window.h"
#include "inputmanager.h"
#include "threading.h"
#include "application.h"
 
using namespace std;
using namespace geeL::memory;

namespace geeL {

	void updateExitStatus(Input& input, RenderWindow& window) {
		if(input.getKeyHold(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window.glWindow, GL_TRUE);
	}



	Application::Application(RenderWindow& window, 
		InputManager& inputManager, 
		ContinuousThread& mainThread,
		Memory& memory)
			: window(window)
			, inputManager(inputManager)
			, memory(memory) {

		inputManager.init(&window);
		addThread(mainThread);
	}


	void Application::run() {
		glfwMakeContextCurrent(0);
		
		initThreads();

		Time& inner = Time();
		long FPS = 10;

		while (!window.shouldClose()) {
			inner.reset();

			glfwPollEvents();
			inputManager.update();

			inner.update();
			long currFPS = std::max(0L, FPS - inner.deltaTimeMS());
			this_thread::sleep_for(chrono::milliseconds(currFPS));

			updateExitStatus(inputManager, window);
		}

		close = true;
		joinThreads();

		window.close();
	}

	void Application::addThread(ContinuousThread& thread) {
		thread.setApplication(*this);
		tempThreads.push_back(&thread);
	}

	bool Application::closing() const {
		return close;
	}

	const ContinuousThread* const Application::getThread(ThreadID id) {
		std::lock_guard<std::mutex> guard(threadLock);

		auto it = threads.find(id);
		if (it != threads.end()) {
			auto* thread = it->second.first;

			return thread;
		}
		
		return nullptr;
	}

	const ContinuousThread* const Application::getCurrentThread() {
		return getThread(this_thread::get_id());
	}

	Time Application::getCurrentTime() {
		auto* thread = getCurrentThread();
		if (thread != nullptr) {
			return thread->getTime();
		}

		return Time();
	}

	memory::Memory& Application::getMemory() {
		return memory;
	}

	void Application::initThreads() {
		for (auto it(tempThreads.begin()); it != tempThreads.end(); it++) {
			ContinuousThread& thread = **it;

			std::thread t = std::move(thread.start());
			ThreadID id = t.get_id();

			threads[id] = pair<ContinuousThread*, std::thread>(&thread, std::move(t));
		}

		tempThreads.clear();
	}

	void Application::joinThreads() {
		for (auto it(threads.begin()); it != threads.end(); it++) {
			std::thread* thread = &it->second.second;

			if (thread->joinable())
				thread->join();
		}

	}

}