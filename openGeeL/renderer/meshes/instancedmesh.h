#ifndef INSTANCEDMESH_H
#define INSTANCEDMESH_H

#include <string>

namespace geeL {

	class Mesh;
	class Shader;
	class SkinnedMesh;
	class StaticMesh;


	class InstancedMesh {

	public:
		InstancedMesh(const Mesh& mesh);

		virtual void draw(const Shader& shader) const = 0;


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
