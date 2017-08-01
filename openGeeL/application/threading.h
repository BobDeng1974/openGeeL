#ifndef GEELTHREADING_H
#define GEELTHREADING_H

namespace geeL {

	//Interface for all objects that can run via a separate thread
	class ThreadedObject {

	public:
		virtual void runStart() {}
		virtual void run() {}
		virtual void runEnd() {}

	};


	//Thread object that runs at a specified framerate
	class ContinuousThread {

	public:
		ContinuousThread(ThreadedObject& obj, long FPS = 60);

		//Returns and starts std::thread that runs containing threaded object
		std::thread start();

	private:
		long FPS;
		ThreadedObject& obj;

		void run();

	};


}

#endif
