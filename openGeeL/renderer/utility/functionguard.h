#ifndef FUNCTIONGUARD_H
#define FUNCTIONGUARD_H

#include <functional>

namespace geeL {

	template<typename T>
	class FunctionGuard {

	public:
		FunctionGuard(T& obj, std::function<void()> enter, std::function<void()> exit);
		~FunctionGuard();

		T* operator->();

	private:
		T& obj;
		std::function<void()> exit;

	};


	template<typename T>
	inline FunctionGuard<T>::FunctionGuard(T& obj, std::function<void()> enter, std::function<void()> exit)
		: obj(obj)
		, exit(exit) {

		enter();
	}

	template<typename T>
	inline FunctionGuard<T>::~FunctionGuard() {
		exit();
	}

	template<typename T>
	inline T* FunctionGuard<T>::operator->() {
		return &obj;
	}

}

#endif
