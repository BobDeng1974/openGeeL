#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <vec3.hpp>
#include <functional>
#include <map>
#include <list>

namespace geeL {

	class SceneCamera;
	class SceneRequester;
	class LightManager;
	class Skybox;
	class Shader;
	class RenderPipeline;
	class SceneShader;
	class MeshRenderer;
	class SkinnedMeshRenderer;
	class StaticModel;
	class Material;
	class MaterialFactory;
	class MeshFactory;
	class Physics;
	class Transform;

	struct ScreenInfo;
	enum class CullingMode;

	class RenderScene {

	public:
		LightManager& lightManager;

		RenderScene(Transform& world, LightManager& lightManager, RenderPipeline& shaderManager, SceneCamera& camera, 
			MeshFactory& meshFactory, MaterialFactory& materialFactory);


		//Update scene information. Should be called once at beginning of frame
		void update();
		
		//Draw all objects that are linked to given shader
		void draw(SceneShader& shader);

		//Draw all those objects with a deferred rendering shader
		void drawDeferred() const;

		//Draw all deferred shaded objects from given camera's perspective
		void drawDeferred(const Camera& camera) const;

		//Draw all those objects with a forward rendering shader
		void drawForward() const;

		//Draw all forward shaded objects from given camera's perspective
		void drawForward(const Camera& camera) const;
	
		//Draw all objects in the scene with given shader
		void drawObjects(const Shader& shader) const;

		//Draw all static objects in the scene with given shader
		void drawStaticObjects(const Shader& shader) const;

		//Draw all skinned objects in the scene with given shader
		void drawSkinnedObjects(const Shader& shader) const;

		void setSkybox(Skybox& skybox);
		void bindSkybox(Shader& shader) const;

		//Draw skybox
		void drawSkybox() const;
		void drawSkybox(const Camera& camera) const;

		void forwardScreenInfo(const ScreenInfo& info);

		void setPhysics(Physics* physics);

		void addRequester(SceneRequester& requester);
		void setCamera(SceneCamera& camera);

		const SceneCamera& getCamera() const;
		SceneCamera& getCamera();

		void AddMeshRenderer(MeshRenderer& renderer);
		void AddMeshRenderer(SkinnedMeshRenderer& renderer);

		void RemoveMeshRenderer(MeshRenderer& renderer);
		void RemoveMeshRenderer(SkinnedMeshRenderer& renderer);

		void iterAllObjects(std::function<void(MeshRenderer&)> function);

		void iterRenderObjects(std::function<void(const MeshRenderer&)> function) const;
		bool iterRenderObjects(SceneShader& shader, std::function<void(const MeshRenderer&)> function) const;

		void iterSkinnedObjects(std::function<void(const MeshRenderer&)> function) const;
		bool iterSkinnedObjects(SceneShader& shader, std::function<void(const SkinnedMeshRenderer&)> function) const;

	private:
		SceneCamera* camera;
		Transform& worldTransform;
		Skybox* skybox;
		Physics* physics;
		MeshFactory& meshFactory;
		RenderPipeline& pipeline;
		MaterialFactory& materialFactory;

		std::list<SceneRequester*> sceneRequester;

		//Objects are indexed by their used shaders (and their transforms id) to allow grouped drawing and 
		//therefore no unnecessary shader programm switching. Objects with multiple materials are linked to
		//all their shaders respectively
		std::map<SceneShader*, std::map<unsigned int, MeshRenderer*>> renderObjects;
		std::map<SceneShader*, std::map<unsigned int, SkinnedMeshRenderer*>> skinnedObjects;

	};
}

#endif