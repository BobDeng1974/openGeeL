#include <algorithm>
#include "../shader/shader.h"
#include "../materials/material.h"
#include "../animation/skeleton.h"
#include "mesh.h"
#include "model.h"

using namespace std;

namespace geeL {


	std::vector<MaterialContainer*> Model::getMaterials() const {
		std::vector<MaterialContainer*> materials;
		materials.reserve(meshCount());

		iterateMeshes([&](const Mesh* mesh) {
			materials.push_back(&mesh->getMaterialContainer());
		});

		return materials;
	}

	void Model::draw() const {
		iterateMeshes([&](const Mesh* mesh) {
			mesh->draw();
		});
	}

	void Model::draw(const map<unsigned int, Material*>& customMaterials) const {
		for (auto it = customMaterials.begin(); it != customMaterials.end(); it++) {
			unsigned int i = it->first;

			Material& mat = *it->second;
			mat.bindTextures();
			mat.bind();

			const Mesh& mesh = getMesh(i);
			mesh.draw();
		}
	}


	StaticModel::~StaticModel() {
		for (auto it = meshes.begin(); it != meshes.end(); it++)
			delete *it;
	}



	StaticMesh& StaticModel::addMesh(StaticMesh* mesh) {
		meshes.push_back(mesh);

		return *meshes.back();
	}


	void StaticModel::iterateMeshes(std::function<void(const Mesh* mesh)> function) const {
		for_each(meshes.begin(), meshes.end(), function);
	}

	unsigned int StaticModel::meshCount() const {
		return meshes.size();
	}



	SkinnedModel::~SkinnedModel() {
		for (auto it = meshes.begin(); it != meshes.end(); it++)
			delete *it;
	}


	void SkinnedModel::updateBones(const Skeleton& skeleton) {
		for (auto it = meshes.begin(); it != meshes.end(); it++) {
			SkinnedMesh& mesh = **it;
			mesh.updateMeshBoneData(skeleton);
		}
	}

	void SkinnedModel::setSkeleton(Skeleton* const skeleton) {
		AnimatedObject::setSkeleton(skeleton);

		//Align bone IDs to those of the skeleton
		for (auto it = meshes.begin(); it != meshes.end(); it++) {
			SkinnedMesh& mesh = **it;

			for (auto et = mesh.bonesBegin(); et != mesh.bonesEnd(); et++) {
				string name = et->first;
				MeshBoneData& data = et->second;

				data.id = this->skeleton->getBoneID(name);
			}
		}
	}


	SkinnedMesh& SkinnedModel::addMesh(SkinnedMesh* mesh) {
		meshes.push_back(mesh);
		return *meshes.back();
	}

	void SkinnedModel::iterateMeshes(std::function<void(const Mesh* mesh)> function) const {
		for_each(meshes.begin(), meshes.end(), function);
	}

	unsigned int SkinnedModel::meshCount() const {
		return meshes.size();
	}

}