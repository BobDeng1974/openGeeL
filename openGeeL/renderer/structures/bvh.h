#ifndef BVH_H
#define BVH_H

#include <list>
#include "utility/listener.h"
#include "group.h"
#include "boundedobject.h"

namespace geeL {

	class MeshRenderer;
	class Transform;


	class BVHNode : public BoundedObject, public Group<MeshRenderer> {

	public:
		BVHNode(BVHNode&& other);
		BVHNode& operator=(BVHNode&& other);
		virtual ~BVHNode() {}

		virtual void onChildChange(BVHNode& child) {}
		virtual void onChildEmpty(BVHNode& child) {}

		virtual void balance(BVHNode& toRemove, BVHNode& toAdd) {}

		virtual void updateSize() = 0;
		virtual void updateSizeAdd(const AABoundingBox& newBox);
		void setParent(BVHNode& parent);

	protected:
		BVHNode* parent;

		BVHNode();
		BVHNode(BVHNode& parent);

	};



	class BVH : public BVHNode {

	public:
		BVH();

		virtual void add(MeshRenderer& node);
		virtual bool remove(MeshRenderer& node);

		virtual void updateSize();
		virtual void balance(BVHNode& toRemove, BVHNode& toAdd);

		virtual void iterElements(std::function<void(MeshRenderer&)> function);
		virtual void iterVisibleElements(const Camera& camera, std::function<void(MeshRenderer&)> function);
		virtual size_t getChildCount() const;

	private:
		BVHNode* base;
		std::list<MeshRenderer*> unboundedChildren;

	};



	class BVHLeaf : public BVHNode, public ChangeListener<Transform> {

	public:
		BVHLeaf();
		BVHLeaf(BVHNode& parent);
		virtual ~BVHLeaf();

		virtual void add(MeshRenderer& node);
		virtual bool remove(MeshRenderer& node);

		virtual void updateSize();

		virtual void iterElements(std::function<void(MeshRenderer&)> function);
		virtual void iterVisibleElements(const Camera& camera, std::function<void(MeshRenderer&)> function);
		virtual size_t getChildCount() const;

		virtual void onChange(const Transform& t);

	private:
		struct SplitPane {
			int axis;
			float pane;
		};

		std::list<MeshRenderer*> group;

		void subdivide();
		SplitPane getSplitHeuristic(const glm::vec3& size);

	};




	class BVHBranch : public BVHNode {

	public:
		BVHBranch();
		BVHBranch(BVHBranch&& other);
		BVHBranch(BVHNode& parent);
		BVHBranch(BVHNode& parent, BVHLeaf& firstChild, BVHLeaf& secondChild);
		virtual ~BVHBranch();

		BVHBranch& operator=(BVHBranch&& other);

		virtual void updateSize();
		virtual void balance(BVHNode& toRemove, BVHNode& toAdd);

		virtual void onChildChange(BVHNode& child);
		virtual void onChildEmpty(BVHNode& child);

		virtual void add(MeshRenderer& node);
		virtual bool remove(MeshRenderer& node);
		

		virtual void iterElements(std::function<void(MeshRenderer&)> function);
		virtual void iterVisibleElements(const Camera& camera, std::function<void(MeshRenderer&)> function);
		virtual size_t getChildCount() const;

	private:
		BVHNode* a;
		BVHNode* b;

		void updateSizeLocal();
		void iter(std::function<void(BVHNode& child)> function);

	};

}

#endif
