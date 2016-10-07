#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <list>

namespace geeL {

	class Camera;
	class LightManager;
	class Skybox;
	class Shader;
	class MeshRenderer;
	class Model;
	class MeshFactory;
	class Transform;
	enum CullingMode;


	class RenderScene {

	public:
		Camera& camera;
		LightManager& lightManager;


		RenderScene(LightManager& lightManager, Camera& camera, MeshFactory& meshFactory);

		void setSkybox(Skybox& skybox);
		void bindSkybox(const Shader& shader) const;


		//Create and add new mesh renderer to scene
		MeshRenderer& AddMeshRenderer(string modelPath, Transform& transform, CullingMode faceCulling);
		

		void draw() const;

		//Draw only the objects in the scene and all with given shader
		void drawObjects(const Shader& shader) const;


	private:
		Skybox* skybox;
		MeshFactory& meshFactory;
		list<MeshRenderer> renderObjects;

		void drawSkybox() const;
		void drawObjects() const;

	};


}




#endif