#ifndef GEEL_APPLICATION_H
#define GEEL_APPLICATION_H

#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <thread>
#include "utility/rendertime.h"

namespace geeL {

	namespace memory {
		class Memory;
	}
	
	class RenderWindow;
	class InputManager;
	class ContinuousThread;
	class ThreadedObject;

	using ThreadID = std::thread::id;


	class Application {

	public:
		Application(RenderWindow& window,
			InputManager& inputManager,
			memory::Memory& memory);

		Application(RenderWindow& window, 
			InputManager& inputManager,
			ContinuousThread& mainThread,
			memory::Memory& memory);

		void run();

		//Add thread to application that will 
		//run independendly during runtime
		void addThread(ContinuousThread& thread);

		//Add a threaded object that will be run by the main thread
		//Note: FPS requirements will be ignored
		void addThreadedObject(ThreadedObject& object);

		bool closing() const;
		void setTargetFPS(long value);

		const ContinuousThread* const getCurrentThread();
		Time getCurrentTime();

		memory::Memory& getMemory();

	private:
		long ms;
		RenderWindow& window;
		InputManager& inputManager;
		memory::Memory& memory;
		Time time;

		AtomicWrapper<bool> close;
		std::mutex threadLock;

		std::list<ContinuousThread*> tempThreads;
		std::list<ThreadedObject*> threadedObjects;
		std::map<ThreadID, std::pair<ContinuousThread*, std::thread>> threads;


		void initThreads();
		void joinThreads();

		const ContinuousThread* const getThread(ThreadID id);
		void iterObjects(std::function<void(ThreadedObject&)> function);

	};

}

#endif
