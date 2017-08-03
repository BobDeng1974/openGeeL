#ifndef GEEL_APPLICATION_H
#define GEEL_APPLICATION_H

#include <map>
#include <list>
#include <thread>
#include "threading.h"

namespace geeL {

	using ThreadID = std::thread::id;

	class Renderer;
	class RenderWindow;
	class RenderThread;
	class InputManager;
	class ContinuousThread;


	class Application {

	public:
		Application(RenderWindow& window, InputManager& inputManager, RenderThread& renderThread);

		void run();
		void addThread(ContinuousThread& thread);

		static bool closing();

		const ContinuousThread* const getThread(ThreadID id);
		const ContinuousThread* const getCurrentThread();

	private:
		RenderWindow& window;
		InputManager& inputManager;
		Renderer& renderer;

		std::list<ContinuousThread*> tempThreads;
		std::map<ThreadID, std::pair<ContinuousThread*, std::thread>> threads;

		void initThreads();
		void joinThreads();

	};

}

#endif
