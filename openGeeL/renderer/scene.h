#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <vec3.hpp>
#include <list>
#include <vector>

namespace geeL {

	class Camera;
	class LightManager;
	class Skybox;
	class Shader;
	class MeshRenderer;
	class StaticModel;
	class Material;
	class MeshFactory;
	class Physics;
	class Transform;

	struct ScreenInfo;
	enum CullingMode;

	class RenderScene {

	public:
		Camera& camera;
		LightManager& lightManager;

		RenderScene(LightManager& lightManager, Camera& camera, MeshFactory& meshFactory, Transform& world);
		~RenderScene();

		void setSkybox(Skybox& skybox);
		void bindSkybox(Shader& shader) const;

		//Create and add new mesh renderer to scene
		MeshRenderer& AddMeshRenderer(std::string modelPath, Transform& transform, 
			CullingMode faceCulling, bool deferred = true, std::string name = "MeshRenderer");

		//Create and add new mesh renderer with custom materials to scene
		MeshRenderer& AddMeshRenderer(std::string modelPath, Transform& transform, 
			std::vector<Material*> materials, CullingMode faceCulling, std::string name = "MeshRenderer");

		//Create and add new mesh renderer to scene
		MeshRenderer& AddSkinnedMeshRenderer(std::string modelPath, Transform& transform,
			CullingMode faceCulling, bool deferred = true, std::string name = "SkinnedMeshRenderer");

		//Create and add new mesh renderer with custom materials to scene
		MeshRenderer& AddSkinnedMeshRenderer(std::string modelPath, Transform& transform,
			std::vector<Material*> materials, CullingMode faceCulling, std::string name = "SkinnedMeshRenderer");

		//Update scene information. Should be called once at beginning of frame
		void update();
		
		//Draw only those objects that are suited for deferred rendering
		void drawDeferred() const;

		//Draw all those objects that are suited not for deferred rendering (with forward rendering)
		void drawForward() const;

		//Draw all objects with forward rendering
		void draw() const;

		//Draw skybox indepentently
		void drawSkybox() const;

		//Draw only the objects in the scene and all with given shader
		void drawObjects(const Shader& shader) const;

		void forwardScreenInfo(const ScreenInfo& info);

		//Translate vector from world to screen space
		glm::vec3 TranslateToScreenSpace(const glm::vec3& vector) const;

		//Translate vector from world to view space
		glm::vec3 TranslateToViewSpace(const glm::vec3& vector) const;

		//Transflate vector from view to world space
		glm::vec3 TranslateToWorldSpace(const glm::vec3& vector) const;

		const glm::vec3& GetOriginInViewSpace() const;
		unsigned int getSkyboxID() const;

		void setPhysics(Physics* physics);

		std::list<MeshRenderer*>::iterator renderObjectsBegin();
		std::list<MeshRenderer*>::iterator renderObjectsEnd();


	private:
		Transform& worldTransform;
		glm::vec3 originViewSpace;
		Skybox* skybox;
		Physics* physics;
		MeshFactory& meshFactory;
		std::list<MeshRenderer*> deferredRenderObjects;
		std::list<MeshRenderer*> forwardRenderObjects;
	
		void drawObjects(const std::list<MeshRenderer*>& objects, bool deferred = true) const;
	};
}

#endif