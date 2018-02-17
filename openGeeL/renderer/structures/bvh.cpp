#include "cameras/camera.h"
#include "meshes/meshrenderer.h"
#include "transformation/transform.h"
#include "utility/vectorextension.h"
#include "bvh.h"

#define MAXCHILDREN 5

using namespace glm;

namespace geeL {

	BVHNode::BVHNode()
		: parent(nullptr) {}

	BVHNode::BVHNode(BVHNode& parent)
		: parent(&parent) {}

	BVHNode::BVHNode(BVHNode && other)

		: parent(other.parent) {

		other.parent = nullptr;
	}

	BVHNode& BVHNode::operator=(BVHNode&& other) {
		if (this != &other) {
			BoundedObject::operator=(std::move(other));

			parent = other.parent;
			other.parent = nullptr;
		}

		return *this;
	}

	void BVHNode::setParent(BVHNode& parent) {
		this->parent = &parent;
	}

	void BVHNode::updateSizeAdd(const AABoundingBox& newBox) {
		if (!aabb.contains(newBox)) {
			aabb.extend(newBox);
			if (parent) parent->updateSizeAdd(aabb);
		}
	}





	BVH::BVH() : base(new BVHLeaf(*this)) {}


	void BVH::add(MeshRenderer& node) {
		const AABoundingBox& box = node.getBoundingBox();

		if (box.isUnbounded())
			unboundedChildren.push_back(&node);
		else
			base->add(node);
	}

	bool BVH::remove(MeshRenderer& node) {
		const AABoundingBox& box = node.getBoundingBox();

		if (box.isUnbounded()) {
			auto it = std::find(unboundedChildren.begin(), unboundedChildren.end(), &node);
			if (it != unboundedChildren.end()) {
				unboundedChildren.erase(it);
				return true;
			}
		}
		else {
			return base->remove(node);
		}

		return false;
	}

	void BVH::updateSize() {
		if (unboundedChildren.size() > 0)
			aabb = AABoundingBox();
		else
			aabb = base->getBoundingBox();
	}

	void BVH::balance(BVHNode& toRemove, BVHNode& toAdd) {
		if (base == &toRemove) {
			delete base;
			base = &toAdd;

			toAdd.setParent(*this);
			updateSize();
		}
	}

	void BVH::iterElements(std::function<void(MeshRenderer&)> function) {
		base->iterElements(function);

		for (auto it(unboundedChildren.begin()); it != unboundedChildren.end(); it++)
			function(**it);
	}

	void BVH::iterVisibleElements(const Camera& camera, std::function<void(MeshRenderer&)> function) {
		base->iterVisibleElements(camera, function);

		for (auto it(unboundedChildren.begin()); it != unboundedChildren.end(); it++)
			function(**it);
	}

	size_t BVH::getChildCount() const {
		return unboundedChildren.size() + base->getChildCount();
	}

	



	BVHLeaf::BVHLeaf() : BVHNode() {}

	BVHLeaf::BVHLeaf(BVHNode& parent)
		: BVHNode(parent) {}


	void BVHLeaf::add(MeshRenderer& node) {
		group.push_back(&node);
		updateSizeAdd(node.getBoundingBox());

		subdivide();

		//Listen to transformational changes of given 
		//renderer and  inform parent node if needed
		Transform& t = node.getTransform();
		t.addListener(*this);
	}

	bool BVHLeaf::remove(MeshRenderer& node) {
		auto it = std::find(group.begin(), group.end(), &node);

		if (it != group.end()) {
			group.erase(it);

			//TODO: remove transform listener

			if (group.size() == 0)
				parent->onChildEmpty(*this);

			return true;
		}

		return false;
	}

	void BVHLeaf::updateSize() {
		AABoundingBox updatedBox;
		for (auto it(group.begin()); it != group.end(); it++) {
			auto& r = **it;
			updatedBox.extend(r.getBoundingBox());
		}

		if (updatedBox != aabb) {
			aabb = updatedBox;
			parent->updateSize();
		}
	}


	void BVHLeaf::iterElements(std::function<void(MeshRenderer&)> function) {
		for (auto it(group.begin()); it != group.end(); it++)
			function(**it);
	}

	void BVHLeaf::iterVisibleElements(const Camera & camera, std::function<void(MeshRenderer&)> function) {
		for (auto it(group.begin()); it != group.end(); it++) {
			MeshRenderer& r = **it;

			if (r.isVisible(camera))
				function(r);
		}
	}

	size_t BVHLeaf::getChildCount() const {
		return group.size();
	}

	void BVHLeaf::onChange(const Transform& t) {
		parent->onChildChange(*this);
	}



	void BVHLeaf::subdivide() {
		if (group.size() > MAXCHILDREN) {
			vec3 boxSize(aabb.getSize());
			SplitPane p = getSplitHeuristic(boxSize);

			BVHLeaf* a = new BVHLeaf();
			BVHLeaf* b = new BVHLeaf();
			BVHBranch* c = new BVHBranch(*parent, *a, *b);

			for (auto it(group.begin()); it != group.end(); it++) {
				auto& childNode = **it;

				vec3 position(childNode.getBoundingBox().getMin());
				float v = position[p.axis];

				if (v < p.pane)
					a->add(childNode);
				else
					b->add(childNode);
			}

			group.clear();
			//TODO: remove transform listeners

			assert(a->getChildCount() > 0);
			assert(b->getChildCount() > 0);

			parent->balance(*this, *c);
		}
	}

	BVHLeaf::SplitPane BVHLeaf::getSplitHeuristic(const glm::vec3& size) {
		SplitPane p;
		vec3 min(std::numeric_limits<float>::max());
		vec3 max(std::numeric_limits<float>::min());

		//Find median
		for (auto it(group.begin()); it != group.end(); it++) {
			auto& node = **it;

			min = VectorExtension::min(min, node.getBoundingBox().getMin());
			max = VectorExtension::max(min, node.getBoundingBox().getMax());
		}

		//Find largest axis of bounding box
		if ((size.x >= size.y) && (size.x >= size.z))
			p.axis = 0;
		else if ((size.y > size.x) && (size.y > size.z))
			p.axis = 1;
		else
			p.axis = 2;

		p.pane = (max[p.axis] + min[p.axis]) / 2;

		return p;
	}





	BVHBranch::BVHBranch() {}

	BVHBranch::BVHBranch(BVHBranch&& other)
		: BVHNode(std::move(other))
		, a(other.a)
		, b(other.b) {
	
		other.a = nullptr;
		other.b = nullptr;
	}

	BVHBranch::BVHBranch(BVHNode& parent) 
		: BVHNode(parent) {}

	BVHBranch::BVHBranch(BVHNode& parent, BVHLeaf& firstChild, BVHLeaf& secondChild)
		: BVHNode(parent)
		, a(&firstChild)
		, b(&secondChild) {
	
		a->setParent(*this);
		b->setParent(*this);
	}

	BVHBranch::~BVHBranch() {
		if (a != nullptr) delete a;
		if (b != nullptr) delete b;
	}

	BVHBranch& BVHBranch::operator=(BVHBranch&& other) {
		if (this != &other) {
			BVHNode::operator=(std::move(other));

			if (a != nullptr) delete a;
			if (b != nullptr) delete b;

			a = other.a;
			b = other.b;

			other.a = nullptr;
			other.b = nullptr;
		}

		return *this;
	}


	void BVHBranch::balance(BVHNode& toRemove, BVHNode& toAdd) {
		BVHNode** c = (a == &toRemove) ? &a : (b == &toRemove) ? &b : nullptr;

		if (c) {
			delete *c;
			*c = &toAdd;

			toAdd.setParent(*this);
			updateSize();
		}
	}


	void BVHBranch::onChildChange(BVHNode& child) {
		if (!aabb.contains(child.getBoundingBox())) {

			//TODO: remove child if it moved outside of bounding box and reinsert it into tree structure
		}
	}

	void BVHBranch::onChildEmpty(BVHNode& child) {
		BVHNode* cc = nullptr;
		BVHNode* oc = nullptr;

		if (a == &child) {
			cc = a;
			oc = b;
		}
		else if (b == &child) {
			cc = b;
			oc = a;
		}

		if (cc) {
			delete cc;
			parent->balance(*this, *oc);
		}
	}

	

	void BVHBranch::add(MeshRenderer& node) {
		//Add node to given child node if it is 
		//encapsulated by the child nodes bounding box
		iter([&node](BVHNode& child) {
			const AABoundingBox& box = child.getBoundingBox();

			if (box.contains(node.getBoundingBox())) {
				child.add(node);
			}
		});

		//Otherwise find closest child node and expand its bounding box
		glm::vec3 nodeCenter(node.getBoundingBox().getCenter());

		bool closestDistance = std::numeric_limits<float>::max();
		BVHNode* closestChild = nullptr;


		iter([&](BVHNode& child) {
			const AABoundingBox& box = child.getBoundingBox();
			float distance = box.distanceCenter(nodeCenter);

			if (distance < closestDistance) {
				closestDistance = distance;
				closestChild = &child;
			}
		});

		if (closestChild)
			closestChild->add(node);
	}

	bool BVHBranch::remove(MeshRenderer& node) {
		//Find child node that should encapsulates given node
		iter([&](BVHNode& child) {
			const AABoundingBox& box = child.getBoundingBox();

			if (box.contains(node.getBoundingBox())) {
				bool removed = child.remove(node);
				if (removed) return true;
			}
		});

		return false;
	}



	void BVHBranch::updateSize() {
		AABoundingBox updatedBox;
		updatedBox.extend(a->getBoundingBox());
		updatedBox.extend(b->getBoundingBox());

		if (updatedBox != aabb) {
			aabb = updatedBox;
			parent->updateSize();
		}
	}

	void BVHBranch::updateSizeLocal() {
		aabb.reset();
		aabb.extend(a->getBoundingBox());
		aabb.extend(b->getBoundingBox());
	}


	void BVHBranch::iter(std::function<void(BVHNode&child)> function) {
		function(*a);
		function(*b);
	}

	void BVHBranch::iterElements(std::function<void(MeshRenderer&)> function) {
		a->iterElements(function);
		b->iterElements(function);
	}

	void BVHBranch::iterVisibleElements(const Camera & camera, std::function<void(MeshRenderer&)> function) {
		const ViewFrustum& frustum = camera.getFrustum();
		IntersectionType intersection = aabb.intersect(frustum);

		if (intersection != IntersectionType::Outside) {
			a->iterVisibleElements(camera, function);
			b->iterVisibleElements(camera, function);
		}
	}

	size_t BVHBranch::getChildCount() const {
		return a->getChildCount() + b->getChildCount();
	}

}