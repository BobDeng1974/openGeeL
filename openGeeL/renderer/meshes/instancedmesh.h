#ifndef INSTANCEDMESH_H
#define INSTANCEDMESH_H

#include <string>

namespace geeL {

	class MaterialContainer;
	class Mesh;
	class Shader;
	class Skeleton;
	class SkinnedMesh;
	class StaticMesh;


	class InstancedMesh {

	public:
		InstancedMesh(const Mesh& mesh);

		virtual void draw(const Shader& shader) const = 0;

		bool operator==(const Mesh& mesh) const;
		bool operator==(const InstancedMesh& mesh) const;

		MaterialContainer& getMaterialContainer() const;
		const std::string& getName() const;

	protected:
		const Mesh& mesh;

	};


	class InstancedStaticMesh : public InstancedMesh {

	public:
		InstancedStaticMesh(const StaticMesh& mesh);

		virtual void draw(const Shader& shader) const;

	};
	

	class InstancedSkinnedMesh : public InstancedMesh {

	public:
		InstancedSkinnedMesh(const SkinnedMesh& mesh, const Skeleton& skeleton);

		virtual void draw(const Shader& shader) const;
		
	private:
		const Skeleton& skeleton;
		const SkinnedMesh& skinnedMesh;

	};

}

#endif
