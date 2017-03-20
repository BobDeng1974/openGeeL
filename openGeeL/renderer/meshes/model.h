#ifndef MODEL_H
#define MODEL_H

#include <functional>
#include <string>
#include <vector>
#include <map>
#include "../animation/animatedobject.h"
#include "mesh.h"

namespace geeL {

	enum class MapType;

	class Camera;
	class LightManager;
	class Material;
	class MaterialFactory;
	class Mesh;
	class SkinnedMesh;
	class StaticMesh;
	class Texture;
	class TextureMap;
	class Transform;

	//Class that represents a single 3D model in memory
	class Model {

	public:
		Model() {}
		Model(std::string path) : path(path) {}

		//Draw models without materials
		virtual void draw() const;

		//Draw models with given custom materials
		virtual void draw(const std::map<unsigned int, Material*>& customMaterials) const;

		virtual void iterateMeshes(std::function<void(const Mesh*)> function) const = 0;
		virtual unsigned int meshCount() const = 0;
		virtual const Mesh& getMesh(unsigned int index) const = 0;

		virtual std::vector<MaterialContainer*> getMaterials() const;

	protected:
		std::string path;
	};


	//Special model that is intended for static drawing
	class StaticModel : public Model {
	
	public:
		StaticModel() : Model() {}
		StaticModel(std::string path) : Model(path) {}
		~StaticModel();

		//Add static mesh. Memory will be managed by this model
		StaticMesh& addMesh(StaticMesh* mesh);

		virtual void iterateMeshes(std::function<void(const Mesh*)> function) const;
		
		virtual unsigned int meshCount() const;
		virtual const Mesh& getMesh(unsigned int index) const;

	private:
		std::vector<StaticMesh*> meshes;
		
	};


	//Special model that is intented for animated drawing
	class SkinnedModel : public Model, public AnimatedObject {

	public:
		SkinnedModel() : Model(), AnimatedObject() {}
		SkinnedModel(std::string path) : Model(path), AnimatedObject() {}
		~SkinnedModel();

		virtual void updateBones(const Skeleton& skeleton);
		virtual void setSkeleton(Skeleton* const skeleton);

		//Add skinned mesh. Memory will be managed by this model
		SkinnedMesh& addMesh(SkinnedMesh* mesh);

		virtual void iterateMeshes(std::function<void(const Mesh*)> function) const;
		void iterateMeshes(std::function<void(const SkinnedMesh*)> function) const;

		virtual unsigned int meshCount() const;
		virtual const Mesh& getMesh(unsigned int index) const;
		virtual const SkinnedMesh& getSkinnedMesh(unsigned int intex) const;

	private:
		std::vector<SkinnedMesh*> meshes;

	};



	inline const Mesh& StaticModel::getMesh(unsigned int index) const {
		return *meshes[index];
	}

	inline unsigned int StaticModel::meshCount() const {
		return meshes.size();
	}

	inline const Mesh& SkinnedModel::getMesh(unsigned int index) const {
		return *meshes[index];
	}

	inline const SkinnedMesh& SkinnedModel::getSkinnedMesh(unsigned int index) const {
		return *meshes[index];
	}

	inline unsigned int SkinnedModel::meshCount() const {
		return meshes.size();
	}

}

#endif
