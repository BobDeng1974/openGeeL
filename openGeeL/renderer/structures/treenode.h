#ifndef TREENODE_H
#define TREENODE_H

#include <list>
#include "boundingbox.h"
#include "group.h"

namespace geeL {

	class Camera;
	class SceneShader;

	template<typename T>
	class TreeNode : public Group<T> {

	public:
		TreeNode();
		TreeNode(TreeNode<T>&& other);
		TreeNode(TreeNode<T>& parent);

		TreeNode<T>& operator=(TreeNode<T>&& other);

		virtual void draw(const Camera& camera, SceneShader& shader) = 0;
		virtual void balance(TreeNode<T>& toRemove, TreeNode<T>& toAdd) = 0;

		virtual void onChildChange(TreeNode<T>& child) {}
		virtual void onChildRemove(TreeNode<T>& child) {}

		virtual bool isLeaf() const = 0;
		virtual bool operator==(const TreeNode<T>& other) const= 0;
		virtual bool operator!=(const TreeNode<T>& other) const;

		const AABoundingBox& getBoundingBox() const;
		virtual void setParent(TreeNode<T>& parent);

	protected:
		std::list<TreeNode<T>*> children;
		TreeNode<T>* parentNode;
		AABoundingBox aabb;

	};


	template<typename T>
	inline TreeNode<T>::TreeNode()
		: parentNode(nullptr) {}

	template<typename T>
	inline TreeNode<T>::TreeNode(TreeNode<T>&& other)
		: aabb(other.aabb)
		, parentNode(other.parentNode)
		, children(std::move(other.children)) {}

	template<typename T>
	inline TreeNode<T>::TreeNode(TreeNode<T>& parent) 
		: parentNode(&parent) {}

	template<typename T>
	inline TreeNode<T>& TreeNode<T>::operator=(TreeNode<T>&& other) {
		if (this != &other) {
			aabb = other.aabb;
			parentNode = other.parentNode;
			children = std::move(other.children);
		}

		return *this;
	}

	template<typename T>
	inline bool TreeNode<T>::operator!=(const TreeNode<T>& other) const {
		return !(*this == other);
	}

	template<typename T>
	inline const AABoundingBox& TreeNode<T>::getBoundingBox() const {
		return aabb;
	}

	template<typename T>
	inline void TreeNode<T>::setParent(TreeNode<T>& parent) {
		parentNode = &parent;
	}

}

#endif