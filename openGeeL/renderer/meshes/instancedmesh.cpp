#include "mesh.h"
#include "instancedmesh.h"

namespace geeL {

	InstancedMesh::InstancedMesh(const Mesh& mesh)
		: mesh(mesh) {}

	bool InstancedMesh::operator==(const Mesh& mesh) const {
		return &this->mesh == &mesh;
	}

	bool InstancedMesh::operator==(const InstancedMesh& mesh) const {
		return &this->mesh == &mesh.mesh;
	}

	MaterialContainer& InstancedMesh::getMaterialContainer() const {
		return mesh.getMaterialContainer();
	}

	const std::string& InstancedMesh::getName() const {
		return mesh.getName();
	}


	InstancedStaticMesh::InstancedStaticMesh(const StaticMesh & mesh)
		: InstancedMesh(mesh) {}

	void InstancedStaticMesh::draw(const Shader& shader) const {
		mesh.draw(shader);
	}


	InstancedSkinnedMesh::InstancedSkinnedMesh(const SkinnedMesh& mesh, const Skeleton& skeleton)
		: InstancedMesh(mesh)
		, skinnedMesh(mesh)
		, skeleton(skeleton) {}

	void InstancedSkinnedMesh::draw(const Shader& shader) const {
		skinnedMesh.updateBones(shader, skeleton);
		mesh.draw(shader);
	}

}