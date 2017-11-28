#include <cassert>
#include <chrono>
#include <limits>
#include "application.h"
#include "threading.h"

using namespace std;

namespace geeL {

	ContinuousSingleThread::ContinuousSingleThread(ThreadedObject& obj) : obj(obj) {}


	std::thread ContinuousThread::start() {
		return std::thread([this]() { run(); });
	}

	const Time& ContinuousThread::getTime() const {
		return time;
	}

	void ContinuousThread::setApplication(const Application& app) {
		assert(this->app == nullptr && "Can't attach thread to multiple applications");

		this->app = &app;
	}


	void ContinuousSingleThread::run() {
		assert(app != nullptr);
		obj.runStart();

		time.reset();
		Time& inner = Time();
		long ms = 1000L / obj.getFPS();

		while (!app->closing()) {
			inner.reset();

			obj.run();

			inner.update();
			long currMS = ms - inner.deltaTimeMS();
			if (currMS > 0L) this_thread::sleep_for(chrono::milliseconds(currMS));

			time.update();
		}

		obj.runEnd();
	}


	ContinuousMultiThread::ContinuousMultiThread() {}

	void ContinuousMultiThread::addObject(ThreadedObject& obj) {
		objects.push_back(&obj);
	}

	void ContinuousMultiThread::run() {
		assert(app != nullptr);

		iterateObjects([this](ThreadedObject& obj) { obj.runStart(); });

		time.reset();
		Time& inner = Time();

		long fps = 0xFFFFFFFFFFFFFFFF;
		iterateObjects([this, &fps](ThreadedObject& obj) {
			long currFPS = obj.getFPS();
			if (currFPS < fps) fps = currFPS;
		});

		long ms = 1000L / fps;


		while (!app->closing()) {
			inner.reset();

			iterateObjects([this](ThreadedObject& obj) { obj.run(); });

			inner.update();
			long currMS = ms - inner.deltaTimeMS();
			if (currMS > 0L) this_thread::sleep_for(chrono::milliseconds(currMS));

			time.update();
		}

		iterateObjects([this](ThreadedObject& obj) { obj.runEnd(); });
	}

	void ContinuousMultiThread::iterateObjects(std::function<void(ThreadedObject&)> function) {
		for (auto it(objects.begin()); it != objects.end(); it++) {
			ThreadedObject& obj = **it;
			function(obj);
		}
	}

}