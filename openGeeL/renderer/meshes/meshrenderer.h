#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <vector>
#include <map>
#include "../sceneobject.h"

namespace geeL {

	class Animator;
	class DefaultMaterialContainer;
	class Material;
	class Model;
	class SceneShader;
	class Shader;
	class Skeleton;
	class SkinnedModel;
	class StaticModel;
	class Transform;

	enum class CullingMode {
		cullNone,
		cullFront,
		cullBack
	};


	//Represents a drawn model in a render scene. Independent from actual model
	class MeshRenderer : public SceneObject {

	public:
		Model* model;
		const CullingMode faceCulling;

		//Constructor for mesh renderer with no assigned model (since it will be drawn instanced)
		MeshRenderer(Transform& transform, SceneShader& shader,
			CullingMode faceCulling = CullingMode::cullFront, std::string name = "MeshRenderer");

		//Constructor for mesh renderer with an unique assigned model
		MeshRenderer(Transform& transform, SceneShader& shader, Model& model,
			CullingMode faceCulling = CullingMode::cullFront, std::string name = "MeshRenderer");

		~MeshRenderer();


		virtual void draw(bool deferred = true) const;
		virtual void draw(const Shader& shader) const;

		

		//Change materials of mesh renderer in numerical order. For clarification:
		//Let materials hold i materials and default materials hold j materials
		//If i < j, then the first i materials will be changed
		//If i > j, then j materials will be change and the remaining (i - j) materials ignored
		void customizeMaterials(std::vector<Material*> materials);

		//Customize material at index i if present in mesh renderer
		void customizeMaterials(Material* material, unsigned int index);

		//Check if the meshes contain materials with non-default materials (meaning: with no default shading)
		bool containsForwardMaterials() const;

		//Check if the meshes contain default materials (meaning: with default shading)
		bool containsDeferredMaterials() const;

		std::map<unsigned int, Material*>::iterator deferredMaterialsBegin();
		std::map<unsigned int, Material*>::iterator deferredMaterialsEnd();

		std::map<unsigned int, Material*>::iterator forwardMaterialsBegin();
		std::map<unsigned int, Material*>::iterator forwardMaterialsEnd();


	protected:
		bool instanced;
		std::map<unsigned int, Material*> deferredMaterials;
		std::map<unsigned int, Material*> forwardMaterials;

		//Init materials with data from the meshes material containers
		void initMaterials(SceneShader& shader);

		//Transform given model with transformation data of this mesh renderer
		virtual void transformMeshes(Model& model, const std::map<unsigned int, Material*>& materials) const;
		virtual void transformMeshes(const Shader* shader) const;

		void cullFaces() const;
		void uncullFaces() const;

	};


	//Special mesh renderer that is intended for use with animated/skinned models
	class SkinnedMeshRenderer : public MeshRenderer {

	public:
		SkinnedMeshRenderer(Transform& transform, SceneShader& shader,
			CullingMode faceCulling = CullingMode::cullFront, std::string name = "SkinnedMeshRenderer");

		SkinnedMeshRenderer(Transform& transform, SceneShader& shader, SkinnedModel& model,
			CullingMode faceCulling = CullingMode::cullFront, std::string name = "SkinnedMeshRenderer");

		~SkinnedMeshRenderer();
		
		Skeleton& getSkeleton();
		SkinnedModel& getSkinnedModel();

		//Add copy of given animator to scene object that will be updated automatically
		template<class T>
		T& addAnimatorComponent(const T& animator);

	private:
		Skeleton* skeleton;
		Animator* animator;
		SkinnedModel* skinnedModel;

		//Load skeleton into shaders of materials
		virtual void transformMeshes(Model& model, const std::map<unsigned int, Material*>& materials) const;
		virtual void transformMeshes(const Shader* shader) const;

	};

}

#endif
