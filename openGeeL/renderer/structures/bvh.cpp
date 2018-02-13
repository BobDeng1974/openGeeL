#include "cameras/camera.h"
#include "utility/vectorextension.h"
#include "bvh.h"

#define MAXCHILDREN 5

using namespace glm;

namespace geeL {

	BVH::BVH() {}

	BVH::BVH(BVH& parent) 
		: TreeNode<MeshNode>(parent) {}

	BVH::~BVH() {
		for (auto it(children.begin()); it != children.end(); it++)
			delete *it;
	}


	void BVH::draw(const Camera& camera, SceneShader& shader) {
		const ViewFrustum& frustum = camera.getFrustum();
		IntersectionType intersection = aabb.intersect(frustum);

		if (intersection != IntersectionType::Outside) {
			for (auto it(children.begin()); it != children.end(); it++) {
				auto& childNode = **it;
				childNode.draw(camera, shader);
			}
		}
	}

	void BVH::onChildChange(TreeNode<MeshNode>& child) {
		if (!aabb.contains(child.getBoundingBox()))
			updateSize();
	}

	void BVH::onChildRemove(TreeNode<MeshNode>& child) {
		BVH* c = dynamic_cast<BVH*>(&child);

		if (c) {
			auto itChild = std::find(children.begin(), children.end(), &child);

			//Given BVH is child of this node and therefore all(both)
			//children have to be BVHs as well
			if (itChild != children.end()) {
				//Find other BVH child
				auto otherChild = children.end();
				for (auto it(children.begin()); it != children.end(); it++) {
					if (it != itChild) {
						otherChild = it;
						break;
					}
				}

				children.clear();
				delete c;

				//Reblace this node with remaining child node in parent node
				if (parentNode)
					parentNode->balance(*this, **otherChild);
				//Otherwise, this node is the root node and we 
				//have to move child data into this node
				else {
					TreeNode::operator=(std::move(**otherChild));
					delete *otherChild;
				}
			}
		}
	}

	

	void BVH::insert(MeshNode& node) {

		//Add node directly to this BVH if it is empty
		if (isLeaf()) {
			addDirect(node);
			return;
		}
		
		for (auto it(children.begin()); it != children.end(); it++) {
			auto& childNode = **it;
			const AABoundingBox& box = childNode.getBoundingBox();

			//Since current child node is a leaf, all other child nodes 
			//must be leaves as well. Therefore we can't add the node
			//to them and have to add it to this BVH
			if (childNode.isLeaf()) {
				addDirect(node);
				return;
			}

			//Add node to given child node if it is 
			//encapsulated by the child nodes bounding box
			if (box.contains(node.getBoundingBox())) {
				bool added = childNode.add(node);
				if(added) return;
			}
		}

		//Otherwise find closest child node and expand its bounding box
		glm::vec3 nodeCenter(node.getBoundingBox().getCenter());

		bool closestDistance = std::numeric_limits<float>::max();
		TreeNode<MeshNode>* closestNode = nullptr;

		for (auto it(children.begin()); it != children.end(); it++) {
			auto& childNode = **it;
			const AABoundingBox& box = childNode.getBoundingBox();
			float distance = box.distanceCenter(nodeCenter);

			if (distance < closestDistance) {
				closestDistance = distance;
				closestNode = &childNode;
			}
		}

		if (closestNode) {
			closestNode->add(node);
			updateSizeLocal();
		}
	}

	bool BVH::add(MeshNode& node) {
		insert(node);
		
		return true;
	}

	bool BVH::remove(MeshNode& node) {
		//Remove node if it is child of this tree
		for (auto it(children.begin()); it != children.end(); it++) {
			auto& childNode = **it;

			if (childNode == node) {
				children.erase(it);

				//This node is now empty and can be removed from tree structure
				if (parentNode && children.size() == 0)
					parentNode->onChildRemove(*this);
				else
					updateSize();

				return true;
			}
		}
		
		//Otherwise find child node that should encapsulates given node
		for (auto it(children.begin()); it != children.end(); it++) {
			auto& n = **it;
			const AABoundingBox& box = n.getBoundingBox();

			if (box.contains(node.getBoundingBox())) {
				bool removed = n.remove(node);
				if(removed) return true;
			}
		}

		return false;
	}


	void BVH::addDirect(MeshNode& node) {
		children.push_back(&node);
		aabb.extend(node.getBoundingBox());

		subdivide();
	}

	void BVH::subdivide() {
		if (children.size() > MAXCHILDREN) {
			vec3 boxSize(aabb.getSize());
			SplitPane p = getSplitHeuristic(boxSize);

			BVH* a = new BVH(*this);
			BVH* b = new BVH(*this);

			for (auto it(children.begin()); it != children.end(); it++) {
				auto& childNode = **it;

				//MeshNode* meshNode = static_cast<MeshNode*>(&childNode);
				MeshNode* meshNode = dynamic_cast<MeshNode*>(&childNode);
				assert(meshNode != nullptr);

				vec3 position(childNode.getBoundingBox().getMin());
				float v = position[p.axis];

				if (v < p.pane)
					a->add(*meshNode);
				else
					b->add(*meshNode);
			}

			children.clear();

			assert(a->getChildCount() > 0);
			assert(b->getChildCount() > 0);

			children.push_back(a);
			children.push_back(b);
		}
	}

	void BVH::updateSize() {
		AABoundingBox updatedBox;
		for (auto it(children.begin()); it != children.end(); it++) {
			auto& childNode = **it;
			updatedBox.extend(childNode.getBoundingBox());
		}

		if (updatedBox != aabb) {
			aabb = updatedBox;
			if (parentNode) parentNode->onChildChange(*this);
		}
	}

	void BVH::updateSizeLocal() {
		aabb.reset();

		for (auto it(children.begin()); it != children.end(); it++) {
			auto& childNode = **it;
			aabb.extend(childNode.getBoundingBox());
		}
	}

	
	void BVH::balance(TreeNode<MeshNode>& toRemove, TreeNode<MeshNode>& toAdd) {
		auto itChild = std::find(children.begin(), children.end(), &toRemove);

		if (itChild != children.end()) {
			children.erase(itChild);
			delete &toRemove;

			children.push_back(&toAdd);
			toAdd.setParent(*this);
			assert(children.size() == 2);

			updateSize();
		}
	}

	bool BVH::isLeaf() const {
		return children.size() == 0;
	}

	bool BVH::operator==(const TreeNode<MeshNode>& other) const {
		return this == &other;
	}


	BVH::SplitPane BVH::getSplitHeuristic(const glm::vec3& size) {
		SplitPane p;
		vec3 min(std::numeric_limits<float>::max());
		vec3 max(std::numeric_limits<float>::min());

		//Find median
		for (auto it(children.begin()); it != children.end(); it++) {
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


}