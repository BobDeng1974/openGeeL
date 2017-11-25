#include "mesh.h"
#include "instancedmesh.h"

namespace geeL {

	MeshInstance::MeshInstance(const Mesh& mesh)
		: mesh(mesh) {}

	bool MeshInstance::operator==(const Mesh& mesh) const {
		return &this->mesh == &mesh;
	}

	bool MeshInstance::operator==(const MeshInstance& mesh) const {
		return &this->mesh == &mesh.mesh;
	}

	MaterialContainer& MeshInstance::getMaterialContainer() const {
		return mesh.getMaterialContainer();
	}

	const std::string& MeshInstance::getName() const {
		return mesh.getName();
	}


	StaticMeshInstance::StaticMeshInstance(const StaticMesh& mesh)
		: MeshInstance(mesh) {}

	void StaticMeshInstance::draw(const Shader& shader) const {
		mesh.draw(shader);
	}


	SkinnedMeshInstance::SkinnedMeshInstance(const SkinnedMesh& mesh, const Skeleton& skeleton)
		: MeshInstance(mesh)
		, skinnedMesh(mesh)
		, skeleton(skeleton) {}

	void SkinnedMeshInstance::draw(const Shader& shader) const {
		skinnedMesh.updateBones(shader, skeleton);
		mesh.draw(shader);
	}

}