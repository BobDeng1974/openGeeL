#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <functional>
#include <list>
#include <map>
#include "sceneobject.h"
#include "materials/material.h"
#include "utility/glguards.h"

namespace geeL {

	class DefaultMaterialContainer;
	class Material;
	class MaterialContainer;
	class Mesh;
	class Model;
	class SceneShader;
	class RenderShader;
	class Skeleton;
	class SkinnedModel;
	class StaticModel;
	class Transform;

	enum class RenderMode {
		Static,
		Skinned,
		Instanced
	};


	//Represents a drawn model in a render scene. Independent from actual model
	class MeshRenderer : public SceneObject {

	public:
		//Constructor for mesh renderer with an unique assigned model
		MeshRenderer(Transform& transform, SceneShader& shader, Model& model,
			CullingMode faceCulling = CullingMode::cullFront, const std::string& name = "MeshRenderer");
		virtual ~MeshRenderer() {}

		//Draw all meshes of with their respective shader and material.
		//Meshes with same shaders will be rendered in groups to avoid
		//unnecessary program switching in GPU
		virtual void draw() const;

		//Only draw those meshes whose materials
		//are linked to given shader
		virtual void draw(SceneShader& shader) const;

		//Draw all meshes and materials exclusively with the given shader.
		virtual void drawExclusive(SceneShader& shader) const;

		//Draw only the meshes without material properties with given shader
		virtual void drawGeometry(const RenderShader& shader) const;


		//Customize material of given mesh (If it is actually part of this mesh renderer)
		void changeMaterial(Material& material, const Mesh& mesh);

		//Customize material of given mesh but keep original material container and 
		//only change its shader (If it is actually part of this mesh renderer)
		void changeMaterial(SceneShader& material, const Mesh& mesh);
		
		void addMaterialChangeListener(std::function<void(MeshRenderer&, Material, Material)> listener);

		void iterate(std::function<void(const Mesh&, const Material&)> function) const;
		void iterateMeshes(std::function<void(const Mesh&)> function) const;
		void iterateMaterials(std::function<void(MaterialContainer&)> function) const;
		void iterateShaders(std::function<void(const SceneShader&)> function) const;
		void iterateShaders(std::function<void(SceneShader&)> function);

		const Model& getModel() const;
		virtual RenderMode getRenderMode() const;

		bool containsShader(SceneShader& shader) const;

	protected:
		const CullingMode faceCulling;

		//Init materials with data from the meshes material containers
		void initMaterials(SceneShader& shader);


		struct MaterialMapping {
			const Mesh* mesh;
			Material material;

			MaterialMapping(const Mesh& mesh, Material material) 
				: mesh(&mesh), material(std::move(material)) {}
			
			bool operator== (const MaterialMapping &rhs) {
				return mesh == rhs.mesh;
			}
		};

		std::map<SceneShader*, std::list<MaterialMapping>> materials;
		std::list<std::function<void(MeshRenderer&, Material, Material)>> materialListeners;

	private:
		Model* model;

	};
}

#endif
