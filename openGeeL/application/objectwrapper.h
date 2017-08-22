#ifndef OBJECTWRAPPER_H
#define OBJECTWRAPPER_H

#include <atomic>
#include <mutex>
#include <iostream>

namespace geeL {

	template<typename T>
	class TempWrapper {

	public:
		TempWrapper(T& obj);
		TempWrapper(T& obj, std::recursive_mutex& mutex);
		virtual ~TempWrapper();

		T* operator->();

	private:
		T* obj;
		std::recursive_mutex* mutex;

	};



	template<typename T>
	class ObjectWrapper {

	public:
		ObjectWrapper(T& obj);

		virtual TempWrapper<T> operator->();

	protected:
		T* obj;

	};



	template<typename T>
	class MutexWrapper : public ObjectWrapper<T> {

	public:
		MutexWrapper(T& obj);

		virtual TempWrapper<T> operator->();

	private:
		std::recursive_mutex mutex;

	};


	template<typename T>
	class AtomicWrapper {

	public:
		AtomicWrapper();
		AtomicWrapper(const T& obj);
		AtomicWrapper(const AtomicWrapper<T>& other);

		T operator()() const;
		operator T() const;
		AtomicWrapper<T>& operator=(const T& o);
		AtomicWrapper<T>& operator=(const AtomicWrapper<T>& o);

	private:
		std::atomic<T> atom;

	};

	

	template<typename T>
	inline TempWrapper<T>::TempWrapper(T & obj) : obj(&obj), mutex(nullptr) {}

	template<typename T>
	inline TempWrapper<T>::TempWrapper(T& obj, std::recursive_mutex& mutex) : obj(&obj), mutex(&mutex) {
		mutex.lock();
	}

	template<typename T>
	inline TempWrapper<T>::~TempWrapper() {
		if(mutex != nullptr) mutex->unlock();
	}

	template<typename T>
	inline T* TempWrapper<T>::operator->() {
		return obj;
	}



	template<typename T>
	inline ObjectWrapper<T>::ObjectWrapper(T& obj) : obj(&obj) {}

	template<typename T>
	inline TempWrapper<T> ObjectWrapper<T>::operator->() {
		return TempWrapper<T>(*obj);
	}

	template<typename T>
	inline MutexWrapper<T>::MutexWrapper(T& obj) : ObjectWrapper<T>(obj) {}

	template<typename T>
	inline TempWrapper<T> MutexWrapper<T>::operator->() {
		return TempWrapper<T>(*obj, mutex);
	}

	


	template<typename T>
	inline AtomicWrapper<T>::AtomicWrapper() : atom(T()) {}

	template<typename T>
	inline AtomicWrapper<T>::AtomicWrapper(const T& obj) : atom(obj) {}

	template<typename T>
	inline AtomicWrapper<T>::AtomicWrapper(const AtomicWrapper<T>& other) : atom(other()) {}


	template<typename T>
	inline T AtomicWrapper<T>::operator()() const {
		return atom.load();
	}

	template<typename T>
	inline AtomicWrapper<T>::operator T() const {
		return atom.load();
	}

	template<typename T>
	inline AtomicWrapper<T>& AtomicWrapper<T>::operator=(const T& o) {
		atom.store(o);

		return *this;
	}

	template<typename T>
	inline AtomicWrapper<T>& AtomicWrapper<T>::operator=(const AtomicWrapper<T>& o) {
		atom.store(o());

		return *this;
	}

}


#endif
