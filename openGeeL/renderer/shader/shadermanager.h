#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <string>
#include <map>

using namespace std;

namespace geeL {

class RenderScene;
class MaterialFactory;
class Shader;

class ShaderManager {

public:
	ShaderManager(const MaterialFactory& factory);

	void staticBind(const RenderScene& scene) const;
	void dynamicBind(const RenderScene& scene) const;

	//Generate a new uniform buffer object and return its ID
	int generateUniformBuffer(int size);

	//Get the binding point of the uniform buffer object that 
	//corresponds to given ID
	int getUniformBindingPoint(int id) const;


private:
	int bindingPointCounter, camID;
	map<int, int> UBObjects;
	const MaterialFactory& factory;

};

}

#endif
