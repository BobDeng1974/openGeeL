#ifndef SINGLEMESHRENDERER_H
#define SINGLEMESHRENDERER_H

#include <functional>
#include <list>
#include "glwrapper/glguards.h"
#include "glwrapper/masking.h"
#include "materials/material.h"
#include "memory/memoryobject.h"
#include "structures/boundingbox.h"
#include "rendermode.h"
#include "sceneobject.h"

namespace geeL {

	class AnimationContainer;
	class Camera;
	class Mesh;
	class Model;
	class RenderShader;
	class SceneShader;
	class Shader;
	class Skeleton;
	class SkinnedMesh;
	class SkinnedModel;
	class StaticMesh;
	class StaticModel;
	class ViewFrustum;


	//Represents a drawn model in a render scene. Independent from actual model
	class SingleMeshRenderer : public SceneObject {

	public:
		virtual ~SingleMeshRenderer();


		virtual void draw(SceneShader& shader) const;

		//Draw all meshes and materials exclusively with the given shader.
		virtual void drawExclusive(SceneShader& shader) const;

		//Draw only the meshes without material properties with given shader
		virtual void drawGeometry(const RenderShader& shader) const;

		//Add render mask to whole mesh renderer.
		void setRenderMask(RenderMask mask);

		Material& getMaterial();
		const Material& getMaterial() const;
		void setMaterial(Material& material);

		SceneShader& getShader();
		const SceneShader& getShader() const;
		void setShader(SceneShader& shader);

		void addMaterialChangeListener(std::function<void(SingleMeshRenderer&, Material, Material)> listener);
		void addDeleteListener(std::function<void(const SingleMeshRenderer&)> listener);

		virtual RenderMode getRenderMode() const = 0;

		//States if currently visible (from given camera POV)
		bool isVisible(const Camera& camera) const;
		bool isVisible(const ViewFrustum& view) const;

		virtual const Mesh& getMesh() const = 0;
		unsigned short getID() const;

		virtual void drawMesh(const Shader& shader) const = 0;

	protected:
		Material material;
		RenderMask mask;
		const CullingMode faceCulling;
		TransformableBoundingBox aabb;

		std::list<std::function<void(SingleMeshRenderer&, Material, Material)>> materialListeners;
		std::list<std::function<void(const SingleMeshRenderer&)>> deleteListeners;

		SingleMeshRenderer(Transform& transform, 
			const Mesh& mesh, 
			SceneShader& shader, 
			MemoryObject<Model> modelData,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "MeshRenderer");

		void drawMask() const;

	private:
		MemoryObject<Model> modelData;
		unsigned int id;

	};


	//Represents a drawn model in a render scene. Independent from actual model
	class SingleStaticMeshRenderer : public SingleMeshRenderer {

	public:
		//Constructor for mesh renderer with an unique assigned model
		SingleStaticMeshRenderer(Transform& transform,
			const StaticMesh& mesh,
			SceneShader& shader,
			MemoryObject<StaticModel> model,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "MeshRenderer");

		virtual RenderMode getRenderMode() const;
		virtual const Mesh& getMesh() const;

		virtual void drawMesh(const Shader& shader) const;

	private:
		const StaticMesh& mesh;

	};


	//Special mesh renderer that is intended for use with animated/skinned models
	class SingleSkinnedMeshRenderer : public SingleMeshRenderer {

	public:
		SingleSkinnedMeshRenderer(Transform& transform,
			const SkinnedMesh& mesh,
			SceneShader& shader,
			MemoryObject<SkinnedModel> model,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "SkinnedMeshRenderer");

		SingleSkinnedMeshRenderer(Transform& transform,
			const SkinnedMesh& mesh,
			SceneShader& shader,
			MemoryObject<SkinnedModel> model,
			std::shared_ptr<Skeleton> skeleton,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "SkinnedMeshRenderer");

		std::shared_ptr<Skeleton> shareSkeleton();
		Skeleton& getSkeleton();
		AnimationContainer& getAnimationContainer();

		virtual RenderMode getRenderMode() const;
		virtual const Mesh& getMesh() const;

		virtual void drawMesh(const Shader& shader) const;

	private:
		const SkinnedMesh & mesh;
		std::shared_ptr<Skeleton> skeleton;
		AnimationContainer& animationContainer;

	};

	

	//Groups mesh renderers of the same type and draw them with
	//same transform and same shader
	class MeshRendererGroup : public SingleMeshRenderer {

	public:
		MeshRendererGroup(Transform& transform,
			SceneShader& shader,
			std::list<SingleMeshRenderer*> renderers,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "MeshRendererGroup");

		virtual void draw(SceneShader& shader) const;
		virtual void drawExclusive(SceneShader& shader) const;

		virtual RenderMode getRenderMode() const ;
		virtual const Mesh& getMesh() const;

		virtual void drawMesh(const Shader& shader) const;

	private:
		RenderMode mode;
		std::list<SingleMeshRenderer*> renderers;

	};

}

#endif
