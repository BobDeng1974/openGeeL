#ifndef UNIFORMBINDINGSTACK_H
#define UNIFORMBINDINGSTACK_H

#include <string>
#include <map>

namespace geeL {

	class Camera;
	class SceneShader;


	//Stack that manages UBO objects on GPU
	class UniformBindingStack {

	public:
		UniformBindingStack();

		//Statically bind camera UBO into shader. 
		void staticBind(const Camera& camera, SceneShader& shader) const;

		//Bind uniform camera information
		void bindCamera(const Camera& camera) const;

		//Generate a new uniform buffer object and return its ID
		int generateUniformBuffer(int size);

		//Get the binding point of the uniform buffer object that 
		//corresponds to given ID
		int getUniformBindingPoint(int id) const;


	private:
		int bindingPointCounter, camID;
		std::map<int, int> UBObjects;

	};

}

#endif
