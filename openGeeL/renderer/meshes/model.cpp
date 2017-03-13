#include <iostream>
#include "../shader/shader.h"
#include "../materials/material.h"
#include "mesh.h"
#include "model.h"


using namespace std;

namespace geeL {


	StaticModel::~StaticModel() {
		for (auto it = meshes.begin(); it != meshes.end(); it++)
			delete *it;
	}

	void StaticModel::draw() const {
		for (auto it = meshes.begin(); it != meshes.end(); it++) {
			const StaticMesh& mesh = **it;
			mesh.draw();
		}
	}

	void StaticModel::draw(const vector<Material*>& customMaterials) const {
		size_t size = customMaterials.size();

		size_t i = 0;
		for (auto it = meshes.begin(); it != meshes.end(); it++) {
			const Mesh& mesh = **it;

			if (i < size) {
				Material& mat = *customMaterials[i];
				mesh.draw(mat);
			}
			else
				mesh.draw();

			i++;
		}
	}

	void StaticModel::draw(const map<unsigned int, Material*>& customMaterials) const {

		for (auto it = customMaterials.begin(); 
			it != customMaterials.end(); it++) {
			
			unsigned int i = it->first;
			Material& mat = *it->second;

			const Mesh& mesh = *meshes[i];
			mesh.draw(mat);
		}
	}



	StaticMesh& StaticModel::addMesh(StaticMesh* mesh) {
		meshes.push_back(mesh);

		return *meshes.back();
	}

	vector<StaticMesh*>::iterator StaticModel::meshesBegin() {
		return meshes.begin();
	}

	vector<StaticMesh*>::iterator StaticModel::meshesEnd() {
		return meshes.end();
	}

	vector<StaticMesh*>::const_iterator StaticModel::meshesBeginConst() const {
		return meshes.begin();
	}

	vector<StaticMesh*>::const_iterator StaticModel::meshesEndConst() const {
		return meshes.end();
	}

	const Mesh& StaticModel::getMesh(unsigned int index) {
		if (index < meshes.size()) {
			return *meshes[index];
		}

		throw std::out_of_range("Index out of range");
	}

	unsigned int StaticModel::meshCount() const {
		return meshes.size();
	}

	std::vector<MaterialContainer*> StaticModel::getMaterials() const {
		std::vector<MaterialContainer*> materials;
		materials.reserve(meshCount());

		for (auto it = meshes.begin(); it != meshes.end(); it++) {
			const Mesh& mesh = **it;

			materials.push_back(&mesh.getMaterialContainer());
		}

		return materials;
	}


	SkinnedModel::~SkinnedModel() {
		for (auto it = meshes.begin(); it != meshes.end(); it++)
			delete *it;

		for (auto it = animations.begin(); it != animations.end(); it++)
			delete *it;
	}

	void SkinnedModel::draw() const {
		for (auto it = meshes.begin(); it != meshes.end(); it++) {
			const Mesh& mesh = **it;
			mesh.draw();
		}
	}

	void SkinnedModel::draw(const vector<Material*>& customMaterials) const {
		size_t size = customMaterials.size();

		size_t i = 0;
		for (auto it = meshes.begin(); it != meshes.end(); it++) {
			const Mesh& mesh = **it;

			if (i < size) {
				Material& mat = *customMaterials[i];
				mesh.draw(mat);
			}
			else
				mesh.draw();

			i++;
		}
	}


	void SkinnedModel::draw(const map<unsigned int, Material*>& customMaterials) const {

		for (auto it = customMaterials.begin();
			it != customMaterials.end(); it++) {

			unsigned int i = it->first;
			Material& mat = *it->second;

			const Mesh& mesh = *meshes[i];
			mesh.draw(mat);
		}
	}

	void SkinnedModel::addAnimation(Animation* animation) {
		animations.push_back(animation);
	}

	SkinnedMesh& SkinnedModel::addMesh(SkinnedMesh* mesh) {
		meshes.push_back(mesh);
		return *meshes.back();
	}

	const Mesh& SkinnedModel::getMesh(unsigned int index) {
		if (index < meshes.size()) {
			return *meshes[index];
		}

		throw std::out_of_range("Index out of range");
	}

	unsigned int SkinnedModel::meshCount() const {
		return meshes.size();
	}

	std::vector<MaterialContainer*> SkinnedModel::getMaterials() const {
		std::vector<MaterialContainer*> materials;
		materials.reserve(meshCount());

		for (auto it = meshes.begin(); it != meshes.end(); it++) {
			const Mesh& mesh = **it;

			materials.push_back(&mesh.getMaterialContainer());
		}

		return materials;
	}
}