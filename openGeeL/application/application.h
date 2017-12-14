#ifndef GEEL_APPLICATION_H
#define GEEL_APPLICATION_H

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

	using ThreadID = std::thread::id;


	class Application {

	public:
		Application(RenderWindow& window, 
			InputManager& inputManager,
			ContinuousThread& mainThread,
			memory::Memory& memory);

		void run();
		void addThread(ContinuousThread& thread);

		bool closing() const;

		const ContinuousThread* const getCurrentThread();
		Time getCurrentTime();

		memory::Memory& getMemory();

	private:
		RenderWindow& window;
		InputManager& inputManager;
		memory::Memory& memory;

		AtomicWrapper<bool> close;
		std::mutex threadLock;
		std::list<ContinuousThread*> tempThreads;
		std::map<ThreadID, std::pair<ContinuousThread*, std::thread>> threads;

		void initThreads();
		void joinThreads();

		const ContinuousThread* const getThread(ThreadID id);

	};

}

#endif
