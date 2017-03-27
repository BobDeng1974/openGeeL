#ifndef RENDERSCENE_H
#define RENDERSCENE_H

#include <vec3.hpp>
#include <functional>
#include <map>
#include <vector>

namespace geeL {

	class Camera;
	class LightManager;
	class Skybox;
	class Shader;
	class ShaderInformationLinker;
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

		RenderScene(Transform& world, LightManager& lightManager, ShaderInformationLinker& shaderManager, Camera& camera, 
			MeshFactory& meshFactory, MaterialFactory& materialFactory);


		void setSkybox(Skybox& skybox);
		void bindSkybox(Shader& shader) const;

		//Create and add new mesh renderer to scene
		void AddMeshRenderer(MeshRenderer& renderer);

		//Create and add new mesh renderer to scene
		void AddSkinnedMeshRenderer(SkinnedMeshRenderer& renderer);

		//Update scene information. Should be called once at beginning of frame
		void update();
		
		//Draw all objects that are linked to given shader
		void draw(SceneShader& shader);

		//Draw all those objects with a deferred rendering shader
		void drawDeferred() const;

		//Draw all those objects with a forward rendering shader
		void drawForward() const;

		//Draw skybox indepentently
		void drawSkybox() const;

		//Draw all objects in the scene with the given shader
		void drawObjects(const Shader& shader) const;

		//Draw all static objects in the scene with the given shader
		void drawStaticObjects(const Shader& shader) const;

		//Draw all skinned objects in the scene with the given shader
		void drawSkinnedObjects(const Shader& shader) const;

		void forwardScreenInfo(const ScreenInfo& info);

		//Translate vector from world to screen space
		glm::vec3 TranslateToScreenSpace(const glm::vec3& vector) const;

		//Translate vector from world to view space
		glm::vec3 TranslateToViewSpace(const glm::vec3& vector) const;

		//Transflate vector from view to world space
		glm::vec3 TranslateToWorldSpace(const glm::vec3& vector) const;

		const glm::vec3& GetOriginInViewSpace() const;

		void setPhysics(Physics* physics);

		const Camera& getCamera() const;
		Camera& getCamera();


		void iterAllObjects(std::function<void(MeshRenderer&)> function);

		void iterRenderObjects(std::function<void(const MeshRenderer&)> function) const;
		bool iterRenderObjects(SceneShader& shader, std::function<void(const MeshRenderer&)> function) const;

		void iterSkinnedObjects(std::function<void(const MeshRenderer&)> function) const;
		bool iterSkinnedObjects(SceneShader& shader, std::function<void(const SkinnedMeshRenderer&)> function) const;

	private:
		Camera* camera;
		Transform& worldTransform;
		glm::vec3 originViewSpace;
		Skybox* skybox;
		Physics* physics;
		MeshFactory& meshFactory;
		ShaderInformationLinker& shaderLinker;
		MaterialFactory& materialFactory;

		//Objects are indexed by their used shaders (and their transforms id) to allow grouped drawing and 
		//therefore no unnecessary shader programm switching. Objects with multiple materials are linked to
		//all their shaders respectively
		std::map<SceneShader*, std::map<unsigned int, MeshRenderer*>> renderObjects;
		std::map<SceneShader*, std::map<unsigned int, SkinnedMeshRenderer*>> skinnedObjects;

	};
}

#endif