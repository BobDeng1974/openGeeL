#include "mesh.h"
#include "instancedmesh.h"

namespace geeL {

	MeshInstance::MeshInstance(const Mesh& mesh, Transform& transform)
		: mesh(mesh)
		, id(0)
		, box(TransformableBoundingBox(mesh.getBoundingBox(), transform)){}


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

	unsigned short MeshInstance::getID() const {
		return id;
	}

	void MeshInstance::setID(unsigned short id) {
		this->id = id;
	}


	StaticMeshInstance::StaticMeshInstance(const StaticMesh& mesh, Transform& transform)
		: MeshInstance(mesh, transform) {}

	void StaticMeshInstance::draw(const Shader& shader) const {
		mesh.draw(shader);
	}


	SkinnedMeshInstance::SkinnedMeshInstance(const SkinnedMesh& mesh, Transform& transform, const Skeleton& skeleton)
		: MeshInstance(mesh, transform)
		, skinnedMesh(mesh)
		, skeleton(skeleton) {}

	void SkinnedMeshInstance::draw(const Shader& shader) const {
		skinnedMesh.updateBones(shader, skeleton);
		mesh.draw(shader);
	}

}