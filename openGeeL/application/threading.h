#ifndef GEELTHREADING_H
#define GEELTHREADING_H

#include <thread>

namespace geeL {

	//Interface for all objects that can run via a separate thread
	class ThreadedObject {

	public:
		ThreadedObject(long FPS = 60);

		virtual void runStart() {}
		virtual void run() {}
		virtual void runEnd() {}

		long getFPS() const;

	private:
		long FPS;

	};


	//Thread object that runs at a specified framerate
	class ContinuousThread {

	public:
		ContinuousThread(ThreadedObject& obj);

		//Returns and starts std::thread that runs containing threaded object
		std::thread start();

	private:
		ThreadedObject& obj;

		void run();

	};



	inline ThreadedObject::ThreadedObject(long FPS) : FPS(FPS) {}

	inline long ThreadedObject::getFPS() const { return FPS; }

}

#endif
