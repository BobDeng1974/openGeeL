#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <vec3.hpp>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <list>
#include <set>
#include "meshes/rendermode.h"
#include "utility/listener.h"
#include "shader/defshading.h"
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
	class UniformBindingStack;
	class SceneShader;
	class MeshRenderer;
	class SkinnedMeshRenderer;
	class StaticModel;
	class Material;
	class MaterialFactory;
	class MeshFactory;
	class Transform;
	class ViewFrustum;

	enum class CullingMode;

	using TransformMapping = std::map<unsigned short, MeshRenderer*>;
	using ShaderMapping    = std::map<SceneShader*, TransformMapping>;


	//Class that holds scene information (Objects, cameras, lights, ...)
	class Scene : public DataEventActuator<MeshRenderer> {

	public:
		Scene(Transform& world, LightManager& lightManager, UniformBindingStack& pipeline, SceneCamera& camera);
		virtual ~Scene();

		void addRequester(SceneRequester& requester);
		void addShader(SceneShader& shader);

		const SceneCamera& getCamera() const;
		SceneCamera& getCamera();
		virtual void setCamera(SceneCamera& camera);

		const LightManager& getLightmanager() const;
		LightManager& getLightmanager();

		virtual MeshRenderer& addMeshRenderer(std::unique_ptr<MeshRenderer> renderer);
		virtual void removeMeshRenderer(MeshRenderer& renderer);

		void setSkybox(Skybox& skybox);

		void iterSceneObjects(std::function<void(SceneObject&)> function);

		void iterRenderObjects(std::function<void(MeshRenderer&)> function) const;
		void iterRenderObjects(SceneShader& shader, std::function<void(const MeshRenderer&)> function) const;
		void iterRenderObjects(ShadingMethod shadingMethod, 
			std::function<void(const MeshRenderer&, SceneShader&)> function) const;
		void iterShaders(std::function<void(SceneShader&)> function);


		//Returns amount of contained objects that are drawn by given shading method
		size_t count(ShadingMethod shadingMethod) const;

		//Returns amount of contained objects that are drawn by given shading methods
		template<typename... ShadingMethods>
		size_t count(ShadingMethod shadingMethod, ShadingMethods ...other) const;

		//States if scene containes objects that are drawn by given shading method
		bool contains(ShadingMethod shadingMethod) const;

		//States if scene containes objects that are drawn by given shading methods
		template<typename... ShadingMethods>
		bool contains(ShadingMethod shadingMethod, ShadingMethods ...other) const;

		bool containsStaticObjects() const;
		bool containsSkinnedObjects() const;

	protected:
		LightManager& lightManager;
		SceneCamera* camera;
		Transform& worldTransform;
		Skybox* skybox;
		UniformBindingStack& pipeline;

		std::list<SceneRequester*> sceneRequester;

		//Objects are indexed by their used shaders, shading method their transforms id to allow grouped drawing and 
		//therefore no unnecessary shader programm switching. Objects with multiple materials are linked to
		//all their shaders respectively
		std::map<ShadingMethod, ShaderMapping> renderObjects;

		//Second indexing structure of mesh renderers that is mainly used for better iteration performance
		std::set<MeshRenderer*> renderers;

		void removeMeshRenderer(MeshRenderer& renderer, SceneShader& shader);
		void updateMeshRenderer(MeshRenderer& renderer, Material oldMaterial, Material newMaterial);

		void iterShaders(std::function<bool(const SceneShader&)> function) const;

	};



	//Scene class that allows consecute drawing and updating of comprised structures
	class RenderScene : public Scene, public ThreadedObject {

	public:
		RenderScene(Transform& world, LightManager& lightManager, UniformBindingStack& pipeline, SceneCamera& camera, 
			MaterialFactory& materialFactory, Input& input);

		virtual MeshRenderer& addMeshRenderer(std::unique_ptr<MeshRenderer> renderer);
		virtual void removeMeshRenderer(MeshRenderer& renderer);


		void init();
		void updateProbes();

		virtual void setCamera(SceneCamera& camera);
		void updateCamera();

		//Tick function that updates scene information of current frame
		virtual void run();
		
		//Update bindings of all contained shaders
		void updateBindings();


		//Draw all objects that contain materials with given shading method
		void draw(ShadingMethod method);

		//Draw all objects that contain materials with given shading method from given camera's perspective
		void draw(ShadingMethod method, const Camera& camera);

		//Draw all objects with forward shading from given camera's perspective
		void drawForwardForced(const Camera& camera, bool forceGamma = false) const;

		//Draw all objects in the scene with given shader and given camera.
		//Note: Scene gets drawn in world space if no camera gets attached
		void drawObjects(SceneShader& shader, const Camera* const camera = nullptr) const;

		//Draw the geometry of all objects in the scene with given shader
		void drawGeometry(const RenderShader& shader) const;

		//Draw all objects in the scene with given shader that use given render mode
		void drawGeometry(const RenderShader& shader, RenderMode mode, 
			const ViewFrustum * const frustum = nullptr) const;

		
		void bindSkybox(RenderShader& shader) const;

		//Draw skybox
		void drawSkybox() const;
		void drawSkybox(const Camera& camera) const;

		//Lock scene for reading and writing
		void lock();
		void unlock();

	private:
		bool initialized = false;
		Input& input;
		MaterialFactory& materialFactory;
		std::mutex mutex;


		void drawDefault(const Camera& camera) const;
		void drawForward(const Camera& camera) const;
		void drawHybrid(const Camera& camera) const;
		void drawTransparent(const Camera& camera) const;

		//Draw all objects whose materials have given shading method
		void draw(ShadingMethod shadingMethod, const Camera& camera, bool updateBindings) const;

		void RenderScene::drawForwardOrdered(ShadingMethod shadingMethod, const Camera& camera,
			bool updateBindings = false) const;

	};



	template<typename ...ShadingMethods>
	inline size_t Scene::count(ShadingMethod shadingMethod, ShadingMethods ...other) const {
		return count(shadingMethod) + count(other...);
	}

	template<typename ...ShadingMethods>
	inline bool Scene::contains(ShadingMethod shadingMethod, ShadingMethods ...other) const {
		return contains(shadingMethod) || contains(other...);
	}

}

#endif