#ifndef MODEL_H
#define MODEL_H

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
	class TextureMap;
	class SkinnedMesh;
	class StaticMesh;
	class Texture;
	class Transform;


	//Class that represents a single 3D model in memory
	class Model {

	public:
		Model() {}
		Model(std::string path) : path(path) {}

		virtual void draw() const = 0;
		virtual void drawInstanced(bool shade = true) const = 0;

		virtual void draw(const std::vector<Material*>& customMaterials) const = 0;
		virtual void draw(const std::map<unsigned int, Material*>& customMaterials) const = 0;

		virtual const Mesh& getMesh(unsigned int index) = 0;
		virtual unsigned int meshCount() const = 0;

		virtual std::vector<MaterialContainer*> getMaterials() const = 0;


	protected:
		std::string path;
	};


	//Special model that is intended for static drawing
	class StaticModel : public Model {
	
	public:
		StaticModel() : Model() {}
		StaticModel::StaticModel(std::string path) : Model(path) {}
		~StaticModel();

		//Draw model without materials
		virtual void draw() const;
		virtual void drawInstanced(bool shade = true) const {}

		virtual void draw(const std::vector<Material*>& customMaterials) const;
		virtual void draw(const std::map<unsigned int, Material*>& customMaterials) const;

		//Add static mesh. Memory will be managed by this model
		StaticMesh& addMesh(StaticMesh* mesh);
		virtual const Mesh& getMesh(unsigned int index);
		virtual unsigned int meshCount() const;

		std::vector<StaticMesh*>::iterator meshesBegin();
		std::vector<StaticMesh*>::iterator meshesEnd();

		std::vector<StaticMesh*>::const_iterator meshesBeginConst() const;
		std::vector<StaticMesh*>::const_iterator meshesEndConst() const;

		virtual std::vector<MaterialContainer*> getMaterials() const;

	private:
		std::vector<StaticMesh*> meshes;
		
	};


	//Special model that is intented for animated drawing
	class SkinnedModel : public Model, public AnimatedObject {

	public:
		SkinnedModel() : Model() {}
		SkinnedModel(std::string path) : Model(path) {}
		~SkinnedModel();

		virtual void draw() const;
		virtual void drawInstanced(bool shade = true) const {}

		virtual void draw(const std::vector<Material*>& customMaterials) const;
		virtual void draw(const std::map<unsigned int, Material*>& customMaterials) const;

		virtual void updateBones(const Skeleton& skeleton);

		//Add static mesh. Memory will be managed by this model
		SkinnedMesh& addMesh(SkinnedMesh* mesh);

		virtual const Mesh& getMesh(unsigned int index);
		virtual unsigned int meshCount() const;

		virtual std::vector<MaterialContainer*> getMaterials() const;

	private:
		std::vector<SkinnedMesh*> meshes;

	};

}

#endif
