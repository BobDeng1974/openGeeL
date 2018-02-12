#ifndef GROUPNODE_H
#define GROUPNODE_H

#include <functional>

namespace geeL {

	template<typename T>
	class Group {

	public:
		virtual bool add(T& node) = 0;
		virtual bool remove(T& node) = 0;

		virtual void iterChildren(std::function<void(T&)> function) = 0;
		virtual size_t getChildCount() const = 0;

	};

}

#endif
