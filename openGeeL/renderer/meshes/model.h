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

		virtual void iterateMeshes(std::function<void(const Mesh* mesh)> function) const = 0;
		virtual unsigned int meshCount() const = 0;

		virtual std::vector<MaterialContainer*> getMaterials() const;

	protected:
		std::string path;

		virtual const Mesh& getMesh(unsigned int index) const = 0;
	};


	//Special model that is intended for static drawing
	class StaticModel : public Model {
	
	public:
		StaticModel() : Model() {}
		StaticModel(std::string path) : Model(path) {}
		~StaticModel();

		//Add static mesh. Memory will be managed by this model
		StaticMesh& addMesh(StaticMesh* mesh);

		virtual void iterateMeshes(std::function<void(const Mesh* mesh)> function) const;
		
		virtual unsigned int meshCount() const;

	protected:
		virtual inline const Mesh& getMesh(unsigned int index) const {
			return *meshes[index];
		}

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

		virtual void iterateMeshes(std::function<void(const Mesh* mesh)> function) const;
		
		virtual unsigned int meshCount() const;

	protected:
		virtual inline const Mesh& getMesh(unsigned int index) const {
			return *meshes[index];
		}

	private:
		std::vector<SkinnedMesh*> meshes;

	};

}

#endif
