#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <string>
#include <map>

using namespace std;

namespace geeL {

	class RenderScene;
	class MaterialFactory;
	class SceneShader;

	//Class that forwards scene information into all registered shaders
	class ShaderManager {

	public:
		ShaderManager(MaterialFactory& factory);

		void staticBind(const RenderScene& scene) const;
		void dynamicBind(const RenderScene& scene) const;

		//Static bind a single shader. Should be called when a new shader is created at runtime
		void staticForwardBind(const RenderScene& scene, SceneShader& shader) const;
		void staticDeferredBind(const RenderScene& scene, SceneShader& shader) const;

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
