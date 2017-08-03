#ifndef GEELTHREADING_H
#define GEELTHREADING_H

#include <thread>
#include "utility/rendertime.h"

namespace geeL {

	class Renderer;


	//Interface for all objects that can run via a separate thread
	class ThreadedObject {

	public:
		ThreadedObject(long FPS = 60);

		virtual void runStart() {}
		virtual void run() = 0;
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

		const Time& getTime() const;

	private:
		ThreadedObject& obj;
		Time time;

		void run();

	};


	//Thread that runs drawing operations of a renderer
	class RenderThread : public ContinuousThread {

	public:
		RenderThread(Renderer& renderer);

		Renderer& getRenderer();

	private:
		Renderer& renderer;

	};



	inline ThreadedObject::ThreadedObject(long FPS) : FPS(FPS) {}

	inline long ThreadedObject::getFPS() const { return FPS; }

}

#endif
