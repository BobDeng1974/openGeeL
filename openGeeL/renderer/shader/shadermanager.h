#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <string>
#include <map>

using namespace std;

namespace geeL {

	class RenderScene;
	class MaterialFactory;
	class Shader;
	class SceneShader;

	//Class that forwards scene information into all registered shaders
	class ShaderInformationLinker {

	public:
		ShaderInformationLinker(MaterialFactory& factory);

		//Statically bind scene information into shader. Should be called once when a new shader is created at runtime
		void staticBind(const RenderScene& scene, SceneShader& shader) const;

		//Dynamically bind scene information into shader during runtime
		void dynamicBind(const RenderScene& scene, SceneShader& shader) const;

		//Bind only the uniform camera information
		void bindCamera(const RenderScene& scene) const;

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
