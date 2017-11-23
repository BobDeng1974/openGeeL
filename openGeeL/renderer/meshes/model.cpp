#include <algorithm>
#include "shader/shader.h"
#include "materials/material.h"
#include "animation/skeleton.h"
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

	void StaticModel::iterateMeshes(std::function<void(const StaticMesh&)> function) const {
		for_each(meshes.begin(), meshes.end(), function);
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