#ifndef GEEL_APPLICATION_H
#define GEEL_APPLICATION_H

#include <list>
#include <thread>

namespace geeL {

	class Renderer;
	class RenderWindow;
	class InputManager;
	class ContinuousThread;

	class Application {

	public:
		Application(RenderWindow& window, InputManager& inputManager, Renderer& renderer);

		void run();
		void addThread(ContinuousThread& thread);

		static bool closing();

	private:
		RenderWindow& window;
		InputManager& inputManager;
		Renderer& renderer;

		std::list<ContinuousThread*> tempThreads;
		std::list<std::thread> threads;

		void initThreads();
		void joinThreads();

	};

}

#endif
