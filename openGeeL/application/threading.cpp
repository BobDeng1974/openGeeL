#include <chrono>
#include "application.h"
#include "renderer.h"
#include "threading.h"

using namespace std;

namespace geeL {

	ContinuousThread::ContinuousThread(ThreadedObject& obj) : obj(obj) {}


	std::thread ContinuousThread::start() {
		return std::thread([this]() { run(); });
	}

	const Time& ContinuousThread::getTime() const {
		return time;
	}


	void ContinuousThread::run() {
		obj.runStart();

		time.reset();
		Time& inner = Time();
		long ms = 1000L / obj.getFPS();

		while (!Application::closing()) {
			inner.reset();

			obj.run();

			inner.update();
			long currMS = ms - inner.deltaTime();
			if (currMS > 0L) this_thread::sleep_for(chrono::milliseconds(currMS));

			time.update();
		}

		obj.runEnd();
	}



	RenderThread::RenderThread(Renderer& renderer) : ContinuousThread(renderer), renderer(renderer) {}


	Renderer& RenderThread::getRenderer() {
		return renderer;
	}

}