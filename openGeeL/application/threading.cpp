#include <thread>
#include <chrono>
#include "utility/rendertime.h"
#include "application.h"
#include "threading.h"

using namespace std;

namespace geeL {

	ContinuousThread::ContinuousThread(ThreadedObject& obj) : obj(obj) {}


	std::thread ContinuousThread::start() {
		return std::thread([this]() { run(); });
	}


	void ContinuousThread::run() {
		obj.runStart();

		Time& time = Time();
		long ms = 1000L / obj.getFPS();

		while (!Application::closing()) {
			time.reset();

			obj.run();

			time.update();
			long currMS = ms - time.deltaTime();
			if (currMS > 0L) this_thread::sleep_for(chrono::milliseconds(currMS));
		}

		obj.runEnd();
	}


}