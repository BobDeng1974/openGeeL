#include "cameras/camera.h"
#include "meshes/ameshrenderer.h"
#include "transformation/transform.h"
#include "meshnode.h"

namespace geeL {

	MeshNode::MeshNode(MeshRenderer& renderer) 
		: renderer(renderer) {

		//Listen to transformational changes of given 
		//renderer and  inform parent node if needed
		Transform& t = renderer.getTransform();
		t.addChangeListener([this](const Transform& transform) {
			if (parentNode)
				parentNode->onChildChange(*this);
		});
	}


	void MeshNode::draw(const Camera& camera, SceneShader& shader) {
		if(renderer.isVisible(camera))
			renderer.draw(shader);
	}

	void MeshNode::balance(TreeNode<MeshNode>& toRemove, TreeNode<MeshNode>& toAdd) {}

	bool MeshNode::isLeaf() const {
		return true;
	}

	bool MeshNode::operator==(const TreeNode<MeshNode>& other) const {
		const MeshNode* node = dynamic_cast<const MeshNode*>(&other);

		if (node != nullptr) {
			const MeshRenderer& r = node->getMeshRenderer();
			return &renderer == &r;
		}

		return false;
	}

	bool MeshNode::add(MeshNode& node) {
		return false;
	}

	bool MeshNode::remove(MeshNode& node) {
		return false;
	}

	void MeshNode::iterChildren(std::function<void(MeshNode&)> function) {
		function(*this);
	}

	size_t MeshNode::getChildCount() const {
		return 0;
	}

	MeshRenderer& MeshNode::getMeshRenderer() {
		return renderer;
	}

	const MeshRenderer& MeshNode::getMeshRenderer() const {
		return renderer;
	}



	OwningMeshNode::OwningMeshNode(std::unique_ptr<MeshRenderer> renderer)
		: MeshNode(*renderer)
		, rendererMemory(renderer.release()) {

		onAdd(this->renderer);
	}

	OwningMeshNode::~OwningMeshNode() {
		onRemove(std::shared_ptr<MeshRenderer>(rendererMemory));
	}

}