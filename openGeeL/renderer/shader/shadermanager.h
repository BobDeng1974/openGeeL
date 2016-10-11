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
	ShaderManager(MaterialFactory& factory);

	void staticBind(const RenderScene& scene) const;
	void dynamicBind(const RenderScene& scene) const;

	//Static bind a single shader. Should be called when a new shader is created at runtime
	void staticBind(const RenderScene& scene, Shader& shader) const;

	//Generate a new uniform buffer object and return its ID
	int generateUniformBuffer(int size);

	//Get the binding point of the uniform buffer object that 
	//corresponds to given ID
	int getUniformBindingPoint(int id) const;


private:
	int bindingPointCounter, camID;
	map<int, int> UBObjects;
	MaterialFactory& factory;

};

}

#endif
