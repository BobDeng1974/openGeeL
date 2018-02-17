#ifndef GROUPNODE_H
#define GROUPNODE_H

#include <functional>

namespace geeL {

	template<typename T>
	class Group {

	public:
		virtual ~Group() {}

		virtual void add(T& node) = 0;
		virtual bool remove(T& node) = 0;
		virtual void replace(T& toRemove, T& toAdd);

		virtual void iterElements(std::function<void(T&)> function) = 0;
		virtual void iterVisibleElements(const Camera& camera, std::function<void(T&)> function) = 0;
		virtual size_t getChildCount() const = 0;

	};


	template<typename T>
	inline void Group<T>::replace(T& toRemove, T& toAdd) {
		remove(toRemove);
		add(toAdd);
	}

}

#endif
