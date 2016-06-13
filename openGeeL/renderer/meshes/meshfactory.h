#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include <map>
#include <string>

using namespace std;

namespace geeL {

class MaterialFactory;
class Model;

class MeshFactory {

public:

	//Creates, initializes and returns a new model from given file path or 
	//returns an existing model if it already uses this file
	Model& CreateModel(string filePath, MaterialFactory& factory);

	map<string, Model>::iterator modelsBegin();
	map<string, Model>::iterator modelsEnd();

private:
	map<string, Model> models;

};

}

#endif 
