#ifndef BVH_H
#define BVH_H


#include "meshnode.h"
#include "treenode.h"

namespace geeL {

	class BVH : public TreeNode<MeshNode> {

	public:
		BVH();
		BVH(BVH&& other);
		BVH(BVH& parent);
		virtual ~BVH();

		BVH& operator=(BVH&& other);

		virtual void onChildChange(TreeNode<MeshNode>& child);
		virtual void onChildRemove(TreeNode<MeshNode>& child);

		virtual bool isLeaf() const;
		virtual bool operator==(const TreeNode<MeshNode>& other) const;

		//Insert a new tree node during runtime
		void insert(MeshNode& node);
		
		virtual bool add(MeshNode& node);
		virtual bool remove(MeshNode& node);
		virtual void balance(TreeNode<MeshNode>& toRemove, TreeNode<MeshNode>& toAdd);

		virtual void iterChildren(std::function<void(MeshNode&)> function);
		virtual void iterVisibleChildren(const Camera& camera, std::function<void(MeshNode&)> function);
		virtual size_t getChildCount() const;

	private:
		struct SplitPane {
			int axis;
			float pane;
		};

		std::list<TreeNode<MeshNode>*> children;

		void addDirect(MeshNode& node);
		void subdivide();
		void updateSize();
		void updateSizeLocal();

		SplitPane getSplitHeuristic(const glm::vec3& size);

	};

}

#endif
