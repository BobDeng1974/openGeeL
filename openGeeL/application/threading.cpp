#include <chrono>
#include <limits>
#include "application.h"
#include "renderer.h"
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


	void ContinuousSingleThread::run() {
		obj.runStart();

		time.reset();
		Time& inner = Time();
		long ms = 1000L / obj.getFPS();

		while (!Application::closing()) {
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
		iterateObjects([this](ThreadedObject& obj) { obj.runStart(); });

		time.reset();
		Time& inner = Time();

		long fps = 0xFFFFFFFFFFFFFFFF;
		iterateObjects([this, &fps](ThreadedObject& obj) {
			long currFPS = obj.getFPS();
			if (currFPS < fps) fps = currFPS;
		});

		long ms = 1000L / fps;


		while (!Application::closing()) {
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