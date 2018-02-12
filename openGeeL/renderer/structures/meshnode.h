#ifndef MESHNODE_H
#define MESHNODE_H

#include <memory>
#include "utility/listener.h"
#include "treenode.h"

namespace geeL {

	class MeshRenderer;


	class MeshNode : public TreeNode {

	public:
		MeshNode(MeshRenderer& renderer);

		virtual void draw(const Camera& camera, SceneShader& shader);

		virtual bool isLeaf() const;
		virtual bool operator==(const TreeNode& other) const;

		virtual bool add(TreeNode& node);
		virtual bool remove(TreeNode& node);

		virtual void iterChildren(std::function<void(TreeNode&)> function);
		virtual size_t getChildCount() const;

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
