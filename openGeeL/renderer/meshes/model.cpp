#include <algorithm>
#include "../shader/shader.h"
#include "../materials/material.h"
#include "../animation/skeleton.h"
#include "mesh.h"
#include "model.h"

using namespace std;

namespace geeL {

	void Model::draw() const {
		iterateMeshes([&](const Mesh& mesh) {
			mesh.draw();
		});
	}


	StaticMesh& StaticModel::addMesh(StaticMesh&& mesh) {
		meshes.push_back(std::move(mesh));

		return meshes.back();
	}


	void StaticModel::iterateMeshes(std::function<void(const Mesh&)> function) const {
		for_each(meshes.begin(), meshes.end(), function);
	}




	void SkinnedModel::updateBones(const Skeleton& skeleton) {
		for (auto it = meshes.begin(); it != meshes.end(); it++) {
			SkinnedMesh& mesh = *it;
			mesh.updateMeshBoneData(skeleton);
		}
	}

	void SkinnedModel::setSkeleton(Skeleton* const skeleton) {
		AnimatedObject::setSkeleton(skeleton);

		//Align bone IDs to those of the skeleton
		for (auto it = meshes.begin(); it != meshes.end(); it++) {
			SkinnedMesh& mesh = *it;

			for (auto et = mesh.bonesBegin(); et != mesh.bonesEnd(); et++) {
				string name = et->first;
				MeshBoneData& data = et->second;

				data.id = this->skeleton->getBoneID(name);
			}
		}
	}

	SkinnedMesh& SkinnedModel::addMesh(SkinnedMesh&& mesh) {
		meshes.push_back(std::move(mesh));
		return meshes.back();
	}

	void SkinnedModel::iterateMeshes(std::function<void(const Mesh&)> function) const {
		for_each(meshes.begin(), meshes.end(), function);
	}

	void SkinnedModel::iterateMeshes(std::function<void(const SkinnedMesh&)> function) const {
		for_each(meshes.begin(), meshes.end(), function);
	}

}