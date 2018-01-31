#ifndef AMESHRENDERER_H
#define AMESHRENDERER_H

#include <functional>
#include <list>
#include "glwrapper/glguards.h"
#include "materialmapping.h"
#include "memory/memoryobject.h"
#include "transformation/boundingbox.h"
#include "sceneobject.h"
#include "rendermode.h"

using namespace geeL::memory;

namespace geeL {

	class Camera;
	class Material;
	class MaterialContainer;
	class Mesh;
	class Model;
	class SceneShader;
	class RenderShader;
	class Transform;


	//Represents a drawn model in a render scene. Independent from actual model
	class MeshRenderer : public SceneObject {

	public:
		virtual ~MeshRenderer();

		//Only draw those meshes whose materials
		//are linked to given shader
		virtual void draw(SceneShader& shader) const;

		//Draw all meshes and materials exclusively with the given shader.
		virtual void drawExclusive(SceneShader& shader) const;

		//Draw only the meshes without material properties with given shader
		virtual void drawGeometry(const RenderShader& shader) const;

		//Add render mask to whole mesh renderer.
		void setRenderMask(RenderMask mask);

		//Add render mask to a single contained mesh. 
		//Note: Mesh will also use mask of whole mesh renderer (If set)
		void setRenderMask(RenderMask mask, const MeshInstance& mesh);

		//Customize material of given mesh (If it is actually part of this mesh renderer)
		virtual void changeMaterial(Material& material, const MeshInstance& mesh);

		//Customize material of given mesh but keep original material container and 
		//only change its shader (If it is actually part of this mesh renderer)
		virtual void changeMaterial(SceneShader& material, const MeshInstance& mesh);

		void addMaterialChangeListener(std::function<void(MeshRenderer&, Material, Material)> listener);
		void addDeleteListener(std::function<void(const MeshRenderer&)> listener);

		virtual void iterate(std::function<void(const MeshInstance&, const Material&)> function) const;
		virtual void iterateMeshes(std::function<void(const MeshInstance&)> function) const;
		virtual void iterateMeshesSafe(std::function<void(const MeshInstance&)> function) const;
		virtual void iterateMaterials(std::function<void(MaterialContainer&)> function) const;
		virtual void iterateShaders(std::function<void(const SceneShader&)> function) const;
		virtual void iterateShaders(std::function<void(SceneShader&)> function);

		size_t getMeshCount() const;
		virtual const MeshInstance* getMesh(const std::string& name) const;
		virtual RenderMode getRenderMode() const = 0;

		//States if currently visible (from given camera POV)
		bool isVisible(const Camera& camera) const;
		virtual bool containsShader(SceneShader& shader) const;

		unsigned short getID() const;

	protected:
		RenderMask mask;
		const CullingMode faceCulling;
		MemoryObject<Model> modelData;
		TransformableBoundingBox aabb;
		
		std::map<SceneShader*, std::list<MaterialMapping>> materials;
		std::list<std::function<void(MeshRenderer&, Material, Material)>> materialListeners;
		std::list<std::function<void(const MeshRenderer&)>> deleteListeners;

		MeshRenderer(Transform& transform,
			MemoryObject<Model> model,
			size_t meshCount,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "MeshRenderer");

		void drawMask(const MaterialMapping& mapping) const;
		virtual MaterialMapping* getMapping(const MeshInstance& mesh);

		void addMesh(MeshInstance* mesh);

	private:
		unsigned short id;

		std::list<MeshInstance*> meshes;

	};

}

#endif
