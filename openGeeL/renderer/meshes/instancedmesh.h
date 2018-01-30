#ifndef INSTANCEDMESH_H
#define INSTANCEDMESH_H

#include <string>
#include "transformation/boundingbox.h"

namespace geeL {

	class MaterialContainer;
	class Mesh;
	class Shader;
	class Skeleton;
	class SkinnedMesh;
	class StaticMesh;
	class Transform;


	class MeshInstance {

	public:
		MeshInstance(const Mesh& mesh, Transform& transform);

		virtual void draw(const Shader& shader) const = 0;

		bool operator==(const Mesh& mesh) const;
		bool operator==(const MeshInstance& mesh) const;

		MaterialContainer& getMaterialContainer() const;
		const std::string& getName() const;

		unsigned short getID() const;
		void setID(unsigned short id);

	protected:
		unsigned short id;
		const Mesh& mesh;
		TransformableBoundingBox box;

	};


	class StaticMeshInstance : public MeshInstance {

	public:
		StaticMeshInstance(const StaticMesh& mesh, Transform& transform);

		virtual void draw(const Shader& shader) const;

	};
	

	class SkinnedMeshInstance : public MeshInstance {

	public:
		SkinnedMeshInstance(const SkinnedMesh& mesh, Transform& transform, const Skeleton& skeleton);

		virtual void draw(const Shader& shader) const;
		
	private:
		const Skeleton& skeleton;
		const SkinnedMesh& skinnedMesh;

	};

}

#endif
