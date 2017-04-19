#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <string>
#include <map>

using namespace std;

namespace geeL {

	class Camera;
	class LightManager;
	class MaterialFactory;
	class RenderScene;
	class Shader;
	class SceneShader;

	//Class that can forward scene information into rendering pipeline
	class RenderPipeline {

	public:
		RenderPipeline(MaterialFactory& factory);

		//Statically bind scene information into shader. Should be called once when shader is created at runtime
		void staticBind(const Camera& camera, const LightManager& lightManager, SceneShader& shader) const;

		//Dynamically bind scene information into shader during runtime
		void dynamicBind(const Camera& camera, const LightManager& lightManager, SceneShader& shader) const;

		//Bind uniform camera information
		void bindCamera(const Camera& camera) const;

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
