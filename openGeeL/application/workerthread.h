#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include "objectwrapper.h"

namespace geeL {

	class WorkerThread {

	public:
		WorkerThread();
		~WorkerThread();

		template<typename ...Jobs>
		void addJob(std::function<void()> job, Jobs ...jobs);
		void addJob(std::function<void()> job);

	private:
		std::queue<std::function<void()>> jobs;
		std::thread thread;
		std::condition_variable waitCondition;
		std::mutex workMutex;
		std::mutex jobMutex;

		AtomicWrapper<bool> hasWork;
		AtomicWrapper<bool> endWork;

		void work();

		template<typename ...Jobs>
		void addJobInternal(std::function<void()> job, Jobs ...jobs);
		void addJobInternal(std::function<void()> job);

	};


	template<typename ...Jobs>
	inline void WorkerThread::addJob(std::function<void()> job, Jobs ...jobs) {
		std::unique_lock<std::mutex> lock(jobMutex);
		bool startWork = (jobs.size() == 0);

		addJobInternal(job);
		addJobInternal(jobs...);

		if (startWork) {
			hasWork = true;
			waitCondition.notify_one();
		}
	}

	template<typename ...Jobs>
	inline void WorkerThread::addJobInternal(std::function<void()> job, Jobs ...jobs) {
		addJobInternal(job);
		addJobInternal(jobs...);
	}

}

#endif
