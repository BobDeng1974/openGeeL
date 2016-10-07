#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <map>
#include <string>
#include <list>

using namespace std;

namespace geeL {

class MaterialFactory;
class Model;
class MeshRenderer;
enum CullingMode;

class MeshFactory {

public:

	MeshFactory(MaterialFactory& factory);

	//Creates, initializes and returns a new model from given file path or 
	//returns an existing model if it already uses this file
	Model& CreateModel(string filePath);

	MeshRenderer& CreateMeshRenderer(Model& model, Transform& transform, CullingMode faceCulling);

	map<string, Model>::iterator modelsBegin();
	map<string, Model>::iterator modelsEnd();

	list<MeshRenderer>::iterator rendererBegin();
	list<MeshRenderer>::iterator rendererEnd();

private:
	MaterialFactory& factory;
	map<string, Model> models;
	list<MeshRenderer> meshRenderer;

};

}

#endif 
