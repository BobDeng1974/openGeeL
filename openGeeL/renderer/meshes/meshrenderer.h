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
	class MeshRenderer : public SceneObject {

	public:
		virtual ~MeshRenderer();


		virtual void draw() const;

		//Draw mesh and its material exclusively with the given shader.
		virtual void drawExclusive(SceneShader& shader) const;

		//Draw mesh without material properties with given shader
		virtual void drawGeometry(const RenderShader& shader) const;

		//Add render mask to whole mesh renderer.
		void setRenderMask(RenderMask mask);

		Material& getMaterial();
		const Material& getMaterial() const;
		void setMaterial(Material& material);

		SceneShader& getShader();
		const SceneShader& getShader() const;
		void setShader(SceneShader& shader);

		void addMaterialChangeListener(std::function<void(MeshRenderer&, Material, Material)> listener);
		void addDeleteListener(std::function<void(const MeshRenderer&)> listener);

		const AABoundingBox& getBoundingBox() const;
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

		std::list<std::function<void(MeshRenderer&, Material, Material)>> materialListeners;
		std::list<std::function<void(const MeshRenderer&)>> deleteListeners;

		MeshRenderer(Transform& transform, 
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
	class StaticMeshRenderer : public MeshRenderer {

	public:
		//Constructor for mesh renderer with an unique assigned model
		StaticMeshRenderer(Transform& transform,
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
	class SkinnedMeshRenderer : public MeshRenderer {

	public:
		SkinnedMeshRenderer(Transform& transform,
			const SkinnedMesh& mesh,
			SceneShader& shader,
			MemoryObject<SkinnedModel> model,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "SkinnedMeshRenderer");

		SkinnedMeshRenderer(Transform& transform,
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
		const SkinnedMesh& mesh;
		std::shared_ptr<Skeleton> skeleton;
		AnimationContainer& animationContainer;

	};

	

	//Groups mesh renderers of the same type and draw them with
	//same transform and same shader
	class MeshRendererGroup : public MeshRenderer {

	public:
		MeshRendererGroup(Transform& transform,
			SceneShader& shader,
			std::list<MeshRenderer*> renderers,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "MeshRendererGroup");

		virtual void draw(SceneShader& shader) const;
		virtual void drawExclusive(SceneShader& shader) const;

		virtual RenderMode getRenderMode() const ;
		virtual const Mesh& getMesh() const;

		virtual void drawMesh(const Shader& shader) const;

	private:
		RenderMode mode;
		std::list<MeshRenderer*> renderers;

	};

}

#endif
