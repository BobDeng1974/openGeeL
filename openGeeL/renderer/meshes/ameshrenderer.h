#ifndef AMESHRENDERER_H
#define AMESHRENDERER_H

#include <functional>
#include <list>
#include "utility/glguards.h"
#include "materialmapping.h"
#include "sceneobject.h"

namespace geeL {

	class Camera;
	class Material;
	class MaterialContainer;
	class Mesh;
	class Model;
	class SceneShader;
	class RenderShader;
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
		MeshRenderer(Transform& transform,
			CullingMode faceCulling = CullingMode::cullFront,
			const std::string& name = "MeshRenderer");

		virtual ~MeshRenderer() {}


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
		void setRenderMask(RenderMask mask, const Mesh& mesh);

		//Customize material of given mesh (If it is actually part of this mesh renderer)
		virtual void changeMaterial(Material& material, const Mesh& mesh);

		//Customize material of given mesh but keep original material container and 
		//only change its shader (If it is actually part of this mesh renderer)
		virtual void changeMaterial(SceneShader& material, const Mesh& mesh);

		void addMaterialChangeListener(std::function<void(MeshRenderer&, Material, Material)> listener);

		virtual void iterate(std::function<void(const Mesh&, const Material&)> function) const;
		virtual void iterateMeshes(std::function<void(const Mesh&)> function) const;
		virtual void iterateMaterials(std::function<void(MaterialContainer&)> function) const;
		virtual void iterateShaders(std::function<void(const SceneShader&)> function) const;
		virtual void iterateShaders(std::function<void(SceneShader&)> function);

		virtual const Mesh* getMesh(const std::string& name) const;
		virtual RenderMode getRenderMode() const = 0;

		//Specify, whether this object should be able to be 
		//filtered out / not drawn if it isn't actually visible
		void setAutomaticFiltering(bool value);

		//States if currently visible (from given camera POV)
		//Note: Will always return true if automatic filtering
		//is disabled (Disabled by default)
		bool isVisible(const Camera& camera) const;
		virtual bool containsShader(SceneShader& shader) const;

	protected:
		RenderMask mask;
		const CullingMode faceCulling;
		std::list<std::function<void(MeshRenderer&, Material, Material)>> materialListeners;
		std::map<SceneShader*, std::list<MaterialMapping>> materials;


		void drawMask(const MaterialMapping& mapping) const;
		virtual MaterialMapping* getMapping(const Mesh& mesh);

	private:
		bool autoFilter = false;

	};

}

#endif
