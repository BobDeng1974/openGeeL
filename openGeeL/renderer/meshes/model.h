#ifndef MODEL_H
#define MODEL_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include "animation/animatedobject.h"
#include "mesh.h"

namespace geeL {

	class Mesh;
	class SkinnedMesh;
	class StaticMesh;


	//Class that represents a single 3D model in memory
	class Model {

	public:
		Model() {}
		Model(const std::string& path) : path(path) {}

		virtual void iterateMeshes(std::function<void(const Mesh&)> function) const = 0;
		const std::string& getPath() const;

	protected:
		std::string path;

	};



	template<typename MeshType>
	class GenericModel : public Model {

	public:
		GenericModel() {}
		GenericModel(const std::string& path) : Model(path) {}

		virtual void iterateMeshes(std::function<void(const Mesh&)> function) const;
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
	class SkinnedModel : public GenericModel<SkinnedMesh>, public AnimatedObject {

	public:
		SkinnedModel() : GenericModel(), AnimatedObject() {}
		SkinnedModel(const std::string& path) : GenericModel(path), AnimatedObject() {}

	};



	inline const std::string& Model::getPath() const {
		return path;
	}

	template<typename MeshType>
	inline void GenericModel<MeshType>::iterateMeshes(std::function<void(const Mesh&)> function) const {
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
		meshes.push_back(std::move(mesh));

		return meshes.back();
	}

	template<typename MeshType>
	inline const MeshType& GenericModel<MeshType>::getMesh(unsigned int index) const {
		return meshes[index];
	}

}

#endif
