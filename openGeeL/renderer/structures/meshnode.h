#ifndef MESHNODE_H
#define MESHNODE_H

#include <memory>
#include "utility/listener.h"
#include "treenode.h"

namespace geeL {

	class MeshRenderer;
	class MeshNode;


	class AMeshNode : public TreeNode<MeshNode> {};


	class MeshNode : public AMeshNode {

	public:
		MeshNode(MeshRenderer& renderer);

		virtual bool isLeaf() const;
		virtual bool operator==(const TreeNode<MeshNode>& other) const;

		virtual bool add(MeshNode& node);
		virtual bool remove(MeshNode& node);
		virtual void balance(TreeNode<MeshNode>& toRemove, TreeNode<MeshNode>& toAdd);

		virtual void iterChildren(std::function<void(MeshNode&)> function);
		virtual void iterVisibleChildren(const Camera& camera, std::function<void(MeshNode&)> function);
		virtual size_t getChildCount() const;

		MeshRenderer& getMeshRenderer();
		const MeshRenderer& getMeshRenderer() const;

	protected:
		MeshRenderer& renderer;

	};


	class OwningMeshNode : public MeshNode, public DataEventActuator<MeshRenderer> {

	public:
		OwningMeshNode(std::unique_ptr<MeshRenderer> renderer);
		~OwningMeshNode();

	private:
		MeshRenderer* rendererMemory;

	};

}

#endif
