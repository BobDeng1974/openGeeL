#ifndef BVH_H
#define BVH_H

#include <list>
#include "treenode.h"

namespace geeL {

	class BVH : public TreeNode {

	public:
		BVH();
		BVH(BVH& parent);
		virtual ~BVH();

		virtual void draw(const Camera& camera, SceneShader& shader);

		virtual bool isLeaf() const;
		virtual bool operator==(const TreeNode& other) const;

		//Insert a new tree node during runtime
		void insert(TreeNode& node);
		
		virtual bool add(TreeNode& node);
		virtual bool remove(TreeNode& node);

		virtual void iterChildren(std::function<void(TreeNode&)> function);
		virtual size_t getChildCount() const;

	protected:
		void remove(BVH& child);
		void balance(BVH& toRemove, BVH& toAdd);

	private:
		struct SplitPane {
			int axis;
			float pane;
		};

		BVH* parent;
		std::list<TreeNode*> children;

		void addDirect(TreeNode& node);
		void subdivide();
		void updateSize();
		void updateSizeLocal();

		SplitPane getSplitHeuristic(const glm::vec3& size);

	};

}

#endif
