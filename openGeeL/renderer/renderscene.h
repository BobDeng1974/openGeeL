#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <vec3.hpp>
#include <functional>
#include <map>
#include <mutex>
#include <list>
#include "threading.h"

namespace geeL {

	class Camera;
	class Input;
	class SceneCamera;
	class SceneObject;
	class SceneRequester;
	class LightManager;
	class Skybox;
	class RenderShader;
	class RenderPipeline;
	class SceneShader;
	class MeshRenderer;
	class SkinnedMeshRenderer;
	class StaticModel;
	class Material;
	class MaterialFactory;
	class MeshFactory;
	class Transform;

	struct ScreenInfo;
	enum class CullingMode;


	//Class that holds scene information (Objects, cameras, lights, ...)
	class Scene {

	public:
		Scene(Transform& world, LightManager& lightManager, RenderPipeline& pipeline, SceneCamera& camera);

		void addRequester(SceneRequester& requester);
		void addShader(SceneShader& shader);

		const SceneCamera& getCamera() const;
		SceneCamera& getCamera();
		void setCamera(SceneCamera& camera);

		const LightManager& getLightmanager() const;
		LightManager& getLightmanager();

		void addMeshRenderer(MeshRenderer& renderer);
		void addMeshRenderer(SkinnedMeshRenderer& renderer);

		void removeMeshRenderer(MeshRenderer& renderer);
		void removeMeshRenderer(SkinnedMeshRenderer& renderer);

		void setSkybox(Skybox& skybox);

		void iterSceneObjects(std::function<void(SceneObject&)> function);
		void iterAllObjects(std::function<void(MeshRenderer&)> function);

		void iterRenderObjects(std::function<void(const MeshRenderer&)> function) const;
		bool iterRenderObjects(SceneShader& shader, std::function<void(const MeshRenderer&)> function) const;

		void iterSkinnedObjects(std::function<void(const MeshRenderer&)> function) const;
		bool iterSkinnedObjects(SceneShader& shader, std::function<void(const SkinnedMeshRenderer&)> function) const;

	protected:
		LightManager& lightManager;
		SceneCamera* camera;
		Transform& worldTransform;
		Skybox* skybox;
		RenderPipeline& pipeline;

		std::list<SceneRequester*> sceneRequester;

		//Objects are indexed by their used shaders (and their transforms id) to allow grouped drawing and 
		//therefore no unnecessary shader programm switching. Objects with multiple materials are linked to
		//all their shaders respectively
		std::map<SceneShader*, std::map<unsigned int, MeshRenderer*>> renderObjects;
		std::map<SceneShader*, std::map<unsigned int, SkinnedMeshRenderer*>> skinnedObjects;


	};



	//Scene class that allows consecute drawing and updating of comprised structures
	class RenderScene : public Scene, public ThreadedObject {

	public:
		RenderScene(Transform& world, LightManager& lightManager, RenderPipeline& pipeline, SceneCamera& camera, 
			const MaterialFactory& materialFactory, Input& input);


		void init();
		void updateProbes();
		void updateCamera();

		//Tick function that updates scene information of current frame
		virtual void run();
		
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
		void drawObjects(SceneShader& shader) const;

		//Draw the geometry of all objects in the scene with given shader
		void drawGeometry(const RenderShader& shader) const;

		//Draw all static objects in the scene with given shader
		void drawStaticObjects(const RenderShader& shader) const;

		//Draw all skinned objects in the scene with given shader
		void drawSkinnedObjects(const RenderShader& shader) const;

		
		void bindSkybox(RenderShader& shader) const;

		//Draw skybox
		void drawSkybox() const;
		void drawSkybox(const Camera& camera) const;

		//Lock scene for reading and writing
		void lock();
		void unlock();

		void forwardScreenInfo(const ScreenInfo& info);

	private:
		bool initialized = false;
		Input& input;
		const MaterialFactory& materialFactory;
		std::mutex mutex;

	};

}

#endif