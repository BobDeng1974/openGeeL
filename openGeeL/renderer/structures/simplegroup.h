#ifndef SIMPLEGROUP_H
#define SIMPLEGROUP_H

#include <list>
#include "group.h"

namespace geeL {

	template<typename T>
	class SimpleGroup : public Group<T> {

	public:
		virtual void add(T& node);
		virtual bool remove(T& node);

		virtual void iterElements(std::function<void(T&)> function);
		virtual void iterVisibleElements(const Camera& camera, std::function<void(T&)> function);
		virtual size_t getChildCount() const;

	private:
		std::list<T*> children;

	};


	template<typename T>
	inline void SimpleGroup<T>::add(T& node) {
		children.push_back(&node);
	}

	template<typename T>
	inline bool SimpleGroup<T>::remove(T& node) {
		auto it = std::find(children.begin(), children.end(), &node);

		if (it != children.end()) {
			children.erase(it);
			return true;
		}

		return false;
	}

	template<typename T>
	inline void SimpleGroup<T>::iterElements(std::function<void(T&)> function) {
		for (auto it(children.begin()); it != children.end(); it++)
			function(**it);
	}

	template<typename T>
	inline void SimpleGroup<T>::iterVisibleElements(const Camera & camera, std::function<void(T&)> function) {
		for (auto it(children.begin()); it != children.end(); it++) {
			T& t = **it;

			if (t.isVisible(camera))
				function(t);
		}
	}

	template<typename T>
	inline size_t SimpleGroup<T>::getChildCount() const {
		return children.size();
	}

}

#endif
