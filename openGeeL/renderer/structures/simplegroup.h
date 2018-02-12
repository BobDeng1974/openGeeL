#ifndef SIMPLEGROUP_H
#define SIMPLEGROUP_H

#include <list>
#include "group.h"

namespace geeL {

	class SimpleGroup : public Group {

	public:
		virtual void add(TreeNode& node);
		virtual bool remove(TreeNode& node);

		virtual void iterChildren(std::function<void(TreeNode&)> function);
		virtual size_t getChildCount() const;

	private:
		std::list<TreeNode*> children;

	};


	inline void SimpleGroup::add(TreeNode& node) {
		children.push_back(&node);
	}

	inline bool SimpleGroup::remove(TreeNode& node) {
		auto it = std::find(children.begin(), children.end(), &node);

		if (it != children.end()) {
			children.erase(it);
			return true;
		}

		return false;
	}

	inline void SimpleGroup::iterChildren(std::function<void(TreeNode&)> function) {
		for (auto it(children.begin()); it != children.end(); it++)
			function(**it);
	}

	inline size_t SimpleGroup::getChildCount() const {
		return children.size();
	}

}

#endif
