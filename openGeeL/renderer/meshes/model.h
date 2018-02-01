#ifndef MODEL_H
#define MODEL_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include "animation/animcontainer.h"
#include "renderer/glstructures.h"
#include "structures/boundingbox.h"
#include "mesh.h"

namespace geeL {

	class Mesh;
	class SkinnedMesh;
	class StaticMesh;


	//Class that represents a single 3D model in memory
	class Model : public GLStructure {

	public:
		Model() {}
		Model(const std::string& path) : path(path) {}
		virtual ~Model() {}

		virtual void initGL();
		virtual void clearGL();

		virtual void iterateMeshes(std::function<void(const Mesh&)> function) const = 0;
		virtual void iterateMeshes(std::function<void(Mesh&)> function) = 0;
		virtual size_t meshCount() const = 0;

		virtual std::string toString() const;
		const std::string& getPath() const;

		const AABoundingBox& getBoundingBox() const;

		void* operator new(size_t size);
		void  operator delete(void* pointer);

	protected:
		AABoundingBox aabb;
		std::string path;

	};



	template<typename MeshType>
	class GenericModel : public Model {

	public:
		GenericModel() {}
		GenericModel(const std::string& path) : Model(path) {}

		virtual void iterateMeshes(std::function<void(const Mesh&)> function) const;
		virtual void iterateMeshes(std::function<void(Mesh&)> function);
		virtual void iterateMeshesGeneric(std::function<void(const MeshType&)> function) const;
		virtual size_t meshCount() const;

		MeshType& addMesh(MeshType&& mesh);
		virtual const MeshType& getMesh(unsigned int index) const;

	protected:
		std::vector<MeshType> meshes;
		
	};


	//Special model that is intended for static drawing
	class StaticModel : public GenericModel<StaticMesh> {
	
	public:
		StaticModel() : GenericModel() {}
		StaticModel(const std::string& path) : GenericModel(path) {}
		
	};


	//Special model that is intented for animated drawing
	class SkinnedModel : public GenericModel<SkinnedMesh>, public AnimationContainer {

	public:
		SkinnedModel() : GenericModel(), AnimationContainer() {}
		SkinnedModel(const std::string& path) : GenericModel(path), AnimationContainer() {}

		virtual void addAnimation(std::unique_ptr<AnimationMemory> animation);

	};

	template<typename MeshType>
	inline void GenericModel<MeshType>::iterateMeshes(std::function<void(const Mesh&)> function) const {
		for_each(meshes.begin(), meshes.end(), function);
	}

	template<typename MeshType>
	inline void GenericModel<MeshType>::iterateMeshes(std::function<void(Mesh&)> function) {
		for_each(meshes.begin(), meshes.end(), function);
	}

	template<typename MeshType>
	inline void GenericModel<MeshType>::iterateMeshesGeneric(std::function<void(const MeshType&)> function) const {
		for_each(meshes.begin(), meshes.end(), function);
	}

	template<typename MeshType>
	inline size_t GenericModel<MeshType>::meshCount() const {
		return meshes.size();
	}

	template<typename MeshType>
	inline MeshType& GenericModel<MeshType>::addMesh(MeshType&& mesh) {
		//Update bounding box oh whole model to fit given mesh as well
		const AABoundingBox& box = mesh.getBoundingBox();
		aabb.update(box);

		meshes.push_back(std::move(mesh));

		return meshes.back();
	}

	template<typename MeshType>
	inline const MeshType& GenericModel<MeshType>::getMesh(unsigned int index) const {
		return meshes[index];
	}

}

#endif
