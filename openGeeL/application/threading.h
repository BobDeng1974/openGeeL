#ifndef GEELTHREADING_H
#define GEELTHREADING_H

#include <functional>
#include <list>
#include <thread>
#include "utility/rendertime.h"

namespace geeL {

	class Application;
	class Renderer;


	//Interface for all objects that can run via a separate thread
	class ThreadedObject {

	public:
		ThreadedObject(long targetFPS = 60);

		virtual void runStart() {}
		virtual void run() = 0;
		virtual void runEnd() {}

		long getFPS() const;

	private:
		long FPS;

	};


	class ContinuousThread {

	public:
		//Returns and starts std::thread that runs containing threaded object
		std::thread start();

		const Time& getTime() const;
		void setApplication(const Application& app);

	protected:
		const Application* app = nullptr;
		Time time;

		virtual void run() = 0;

	};


	//Thread that runs one threaded object at its specified specified framerate
	class ContinuousSingleThread : public ContinuousThread {

	public:
		ContinuousSingleThread(ThreadedObject& obj);

	protected:
		virtual void run();

	private:
		ThreadedObject& obj;

	};


	//Thread that runs several threaded object at lowest specified framerate
	class ContinuousMultiThread : public ContinuousThread {

	public:
		ContinuousMultiThread();

		void addObject(ThreadedObject& obj);

	protected:
		virtual void run();

	private:
		std::list<ThreadedObject*> objects;

		void iterateObjects(std::function<void(ThreadedObject&)> function);

	};


	inline ThreadedObject::ThreadedObject(long FPS) : FPS(FPS) {}

	inline long ThreadedObject::getFPS() const { return FPS; }


}

#endif
