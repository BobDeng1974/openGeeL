#ifndef TREENODE_H
#define TREENODE_H

#include "boundingbox.h"
#include "group.h"

namespace geeL {

	class Camera;
	class SceneShader;


	class TreeNode : public Group<TreeNode> {

	public:
		virtual void draw(const Camera& camera, SceneShader& shader) = 0;

		virtual bool isLeaf() const = 0;
		virtual bool operator==(const TreeNode& other) const= 0;
		virtual bool operator!=(const TreeNode& other) const;

		const AABoundingBox& getBoundingBox() const;

	protected:
		AABoundingBox aabb;

	};



	inline const AABoundingBox& TreeNode::getBoundingBox() const {
		return aabb;
	}

	inline bool TreeNode::operator!=(const TreeNode& other) const {
		return !(*this == other);
	}

}

#endif