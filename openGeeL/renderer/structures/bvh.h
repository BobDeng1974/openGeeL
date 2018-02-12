#ifndef BVH_H
#define BVH_H

#include <list>
#include "meshnode.h"
#include "treenode.h"

namespace geeL {

	class BVH : public TreeNode<MeshNode> {

	public:
		BVH();
		BVH(BVH& parent);
		virtual ~BVH();

		virtual void draw(const Camera& camera, SceneShader& shader);

		virtual bool isLeaf() const;
		virtual bool operator==(const TreeNode<MeshNode>& other) const;

		//Insert a new tree node during runtime
		void insert(MeshNode& node);
		
		virtual bool add(MeshNode& node);
		virtual bool remove(MeshNode& node);

		void iterVisibleChildren(const Camera& camera, std::function<void(MeshNode&)> function);
		virtual void iterChildren(std::function<void(MeshNode&)> function);
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
		std::list<TreeNode<MeshNode>*> children;

		void addDirect(MeshNode& node);
		void subdivide();
		void updateSize();
		void updateSizeLocal();

		SplitPane getSplitHeuristic(const glm::vec3& size);

	};

}

#endif
