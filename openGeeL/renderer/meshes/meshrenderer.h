#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <functional>
#include <vector>
#include <map>
#include "../sceneobject.h"
#include "../materials/material.h"

namespace geeL {

	class DefaultMaterialContainer;
	class Material;
	class MaterialContainer;
	class Mesh;
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

		//Draw all meshes of with their respective shader and material.
		//Meshes with same shaders will be rendered in groups to avoid
		//unnecessary program switching in GPU
		virtual void draw() const;

		//Only draw those meshes whose materials
		//are linked to given shader
		virtual void draw(const SceneShader& shader) const;

		//Draw all meshes exclusively with the given shader. No material properties will be used
		virtual void drawExclusive(const Shader& shader) const;

		//Customize material of given mesh (If it is actually part of this mesh renderer)
		void changeMaterial(Material&& material, const Mesh& mesh);

		
		void iterateMaterials(std::function<void(MaterialContainer&)> function) const;
		void iterateShaders(std::function<void(const SceneShader&)> function) const;

		const Model& getModel() const;
		virtual RenderMode getRenderMode() const;

	protected:
		const CullingMode faceCulling;

		//Init materials with data from the meshes material containers
		void initMaterials(SceneShader& shader);

		void cullFaces() const;
		void uncullFaces() const;


		struct MaterialMapping {
			const Mesh* mesh;
			Material material;

			MaterialMapping(const Mesh& mesh, Material material) 
				: mesh(&mesh), material(std::move(material)) {}
			
			bool operator== (const MaterialMapping &rhs) {
				return mesh == rhs.mesh;
			}
		};

		std::map<const SceneShader*, std::list<MaterialMapping>> materials;

	private:
		Model* model;

	};
}

#endif
