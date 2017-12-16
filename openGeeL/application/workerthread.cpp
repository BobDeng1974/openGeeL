#include <iostream>
#include "workerthread.h"

using namespace std;

namespace geeL {

	WorkerThread::WorkerThread() 
		: hasWork(false)
		, endWork(false) {

		thread = std::thread(([this]() { this->work(); }));
	}

	WorkerThread::~WorkerThread()  {
		hasWork = true;
		endWork = true;
		waitCondition.notify_one();

		unique_lock<mutex> lock(workMutex);
		waitCondition.wait(lock, [this] { return !this->hasWork; });

		thread.join();
	}



	void WorkerThread::addJob(std::function<void()> job) {
		unique_lock<mutex> lock(jobMutex);
		bool startWork = (jobs.size() == 0);
		
		jobs.push(job);

		if (startWork) {
			hasWork = true;
			waitCondition.notify_one();
		}
	}

	void WorkerThread::addJobInternal(std::function<void()> job) {
		jobs.push(job);
	}

	void WorkerThread::work() {

		while (!endWork) {
			unique_lock<mutex> lock(workMutex);
			waitCondition.wait(lock, [this] { return this->hasWork; });

			unique_lock<mutex> jobLock(jobMutex);

			//Process work queue
			while (!jobs.empty()) {
				auto function = jobs.front();

				jobLock.unlock();
				function();
				jobLock.lock();

				jobs.pop();
			}

			jobLock.unlock();
			lock.unlock();

			hasWork = false;

			waitCondition.notify_one();
		}
	}
	
}