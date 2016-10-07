#include "mesh.h"
#include "model.h"
#include "meshfactory.h"
#include "meshrenderer.h"

namespace geeL {

	MeshFactory::MeshFactory(MaterialFactory& factory) : factory(factory) {}


	Model& MeshFactory::CreateModel(string filePath) {
		if (models.find(filePath) == models.end()) {
			models[filePath] = Model(filePath);
			models[filePath].loadModel(factory);
		}

		return models[filePath];
	}

	MeshRenderer& MeshFactory::CreateMeshRenderer(Model& model, Transform& transform, CullingMode faceCulling) {
		meshRenderer.push_back(MeshRenderer(transform, model, faceCulling));

		return meshRenderer.back();
	}

	map<string, Model>::iterator MeshFactory::modelsBegin() {
		return models.begin();
	}

	map<string, Model>::iterator MeshFactory::modelsEnd() {
		return models.end();
	}

	list<MeshRenderer>::iterator MeshFactory::rendererBegin() {
		return meshRenderer.begin();
	}

	list<MeshRenderer>::iterator MeshFactory::rendererEnd() {
		return meshRenderer.end();
	}

}