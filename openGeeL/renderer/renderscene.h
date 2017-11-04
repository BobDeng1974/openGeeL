#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <vec3.hpp>
#include <functional>
#include <map>
#include <mutex>
#include <list>
#include <set>
#include "threading.h"
#include "shader/defshading.h"

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

	using TransformMapping = std::map<unsigned int, MeshRenderer*>;
	using ShaderMapping    = std::map<SceneShader*, TransformMapping>;


	//Class that holds scene information (Objects, cameras, lights, ...)
	class Scene {

	public:
		Scene(Transform& world, LightManager& lightManager, RenderPipeline& pipeline, SceneCamera& camera);

		void addRequester(SceneRequester& requester);
		void addShader(SceneShader& shader);

		const SceneCamera& getCamera() const;
		SceneCamera& getCamera();
		virtual void setCamera(SceneCamera& camera);

		const LightManager& getLightmanager() const;
		LightManager& getLightmanager();

		void addMeshRenderer(MeshRenderer& renderer);
		void removeMeshRenderer(MeshRenderer& renderer);

		void setSkybox(Skybox& skybox);

		void iterSceneObjects(std::function<void(SceneObject&)> function);

		void iterRenderObjects(std::function<void(MeshRenderer&)> function) const;
		void iterRenderObjects(SceneShader& shader, std::function<void(const MeshRenderer&)> function) const;
		void iterRenderObjects(ShadingMethod shadingMethod, 
			std::function<void(const MeshRenderer&, SceneShader&)> function) const;


		//Returns amount of contained objects that are drawn by given shading method
		size_t count(ShadingMethod shadingMethod) const;

		//Returns amount of contained objects that are drawn by given shading methods
		template<typename... ShadingMethods>
		size_t count(ShadingMethod shadingMethod, ShadingMethods ...other) const;

	protected:
		LightManager& lightManager;
		SceneCamera* camera;
		Transform& worldTransform;
		Skybox* skybox;
		RenderPipeline& pipeline;

		std::list<SceneRequester*> sceneRequester;

		//Objects are indexed by their used shaders, shading method their transforms id to allow grouped drawing and 
		//therefore no unnecessary shader programm switching. Objects with multiple materials are linked to
		//all their shaders respectively
		std::map<ShadingMethod, ShaderMapping> renderObjects;

		//Second indexing structure of mesh renderers that is mainly used for better iteration performance
		std::set<MeshRenderer*> renderers;

		void removeMeshRenderer(MeshRenderer& renderer, SceneShader& shader);
		void updateMeshRenderer(MeshRenderer& renderer, Material oldMaterial, Material newMaterial);

	};



	//Scene class that allows consecute drawing and updating of comprised structures
	class RenderScene : public Scene, public ThreadedObject {

	public:
		RenderScene(Transform& world, LightManager& lightManager, RenderPipeline& pipeline, SceneCamera& camera, 
			const MaterialFactory& materialFactory, Input& input);


		void init();
		void updateProbes();

		virtual void setCamera(SceneCamera& camera);
		void updateCamera();

		//Tick function that updates scene information of current frame
		virtual void run();
		
		//Draw all objects whose materials have given shading method
		void draw(ShadingMethod shadingMethod, const Camera& camera) const;

		//Draw all those objects with default material (Deferred shading)
		void drawDefault() const;

		//Draw all objects with default material from given camera's perspective
		//(Deferred shading)
		void drawDefault(const Camera& camera) const;


		void drawGeneric() const;

		//Draw all objects with generic materials from given camera's perspective
		//(Forward shading)
		void drawGeneric(const Camera& camera) const;

		//Draw all objects with default material from given camera's perspective
		//(Forward shading)
		void drawGenericForced(const Camera& camera, bool forceGamma = false) const;

		//Draw all those objects that have forward shaded materials
		void drawForward() const;

		//Draw all objects that have forward shaded materials from given camera's perspective
		void drawForward(const Camera& camera) const;

		//Draw all those objects that have order-dependent transparency
		void drawTransparentOD() const;

		//Draw all objects that have order-dependent transparency from given camera's perspective
		void drawTransparentOD(const Camera& camera) const;

		//Draw all those objects that have order-independent transparency
		void drawTransparentOID() const;

		//Draw all objects that have order-independent transparency from given camera's perspective
		void drawTransparentOID(const Camera& camera) const;

		//Draw all objects in the scene with given shader and given camera.
		//Note: Scene gets drawn in world space if no camera gets attached
		void drawObjects(SceneShader& shader, const Camera* const camera = nullptr) const;

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

		void RenderScene::drawGeneric(ShadingMethod shadingMethod, const Camera& camera) const;
		void RenderScene::drawForward(ShadingMethod shadingMethod, const Camera& camera) const;
		void RenderScene::drawForwardOrdered(ShadingMethod shadingMethod, const Camera& camera) const;

	};



	template<typename ...ShadingMethods>
	inline size_t Scene::count(ShadingMethod shadingMethod, ShadingMethods ...other) const {
		return count(shadingMethod) + count(other...);
	}

}

#endif