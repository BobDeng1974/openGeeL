#include <iostream>
#include "../shader/shader.h"
#include "mesh.h"
#include "model.h"

using namespace std;

namespace geeL {

	void Model::draw(bool shade) const {
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].draw(shade);
	}

	void Model::drawInstanced(bool shade) const {
		//TODO: implement this
	}

	void Model::draw(vector<Material*> customMaterials) const {
		size_t size = customMaterials.size();

		for (unsigned int i = 0; i < meshes.size(); i++) {
			if (i < size) {
				Material& mat = *customMaterials[i];
				meshes[i].draw(mat);
			}
			else
				meshes[i].draw();
		}
	}

	void Model::draw(map<unsigned int, Material*> customMaterials) const {

		for (map<unsigned int, Material*>::iterator it = customMaterials.begin(); 
			it != customMaterials.end(); it++) {
			
			unsigned int i = it->first;
			Material& mat = *it->second;
			meshes[i].draw(mat);
		}
	}

	void Model::addMesh(Mesh mesh) {
		meshes.push_back(mesh);
	}

	vector<Mesh>::iterator Model::meshesBegin() {
		return meshes.begin();
	}

	vector<Mesh>::iterator Model::meshesEnd() {
		return meshes.end();
	}

	const Mesh& Model::getMesh(unsigned int index) {
		if (index < meshes.size()) {
			return meshes[index];
		}

		throw std::out_of_range("Index out of range");
	}

	int Model::meshCount() const {
		return meshes.size();
	}

}