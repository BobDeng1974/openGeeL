#ifndef TREENODE_H
#define TREENODE_H

#include "boundingbox.h"
#include "group.h"

namespace geeL {

	class Camera;
	class SceneShader;

	template<typename T>
	class TreeNode : public Group<T> {

	public:
		virtual void draw(const Camera& camera, SceneShader& shader) = 0;

		virtual bool isLeaf() const = 0;
		virtual bool operator==(const TreeNode<T>& other) const= 0;
		virtual bool operator!=(const TreeNode<T>& other) const;

		const AABoundingBox& getBoundingBox() const;

	protected:
		AABoundingBox aabb;

	};


	template<typename T>
	inline bool TreeNode<T>::operator!=(const TreeNode<T>& other) const {
		return !(*this == other);
	}

	template<typename T>
	inline const AABoundingBox& TreeNode<T>::getBoundingBox() const {
		return aabb;
	}

}

#endif