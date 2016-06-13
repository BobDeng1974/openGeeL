#include "mesh.h"
#include "model.h"
#include "meshfactory.h"

namespace geeL {

	Model& MeshFactory::CreateModel(string filePath, MaterialFactory& factory) {
		if (models.find(filePath) == models.end()) {
			models[filePath] = Model(filePath);
			models[filePath].loadModel(factory);
		}

		return models[filePath];
	}

	map<string, Model>::iterator MeshFactory::modelsBegin() {
		return models.begin();
	}

	map<string, Model>::iterator MeshFactory::modelsEnd() {
		return models.end();
	}

}