#ifndef MESH_H
#define MESH_H

#include <vec2.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include <string>
#include <vector>
#include <map>

#define BONECOUNT 3

namespace geeL {

	class Material;
	class MaterialContainer;
	class Skeleton;


	//Single vertex of a mesh
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;
	};


	//Vertex with associated bone data
	struct SkinnedVertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;

		unsigned int IDs[BONECOUNT];
		float weights[BONECOUNT];

		void addBone(unsigned int id, float weight) {
			//Find unused spot in arrays
			for (unsigned int i = 0; i < BONECOUNT; i++) {
				if (weights[i] == 0.f) {
					IDs[i] = id;
					weights[i] = weight;
					return;
				}
			}
		}
	};


	//Single bone in meshes skeleton
	struct MeshBoneData {
		unsigned int id;
		glm::mat4 offsetMatrix;
		glm::mat4 transform;

	};


	//Base class for all mesh containers
	class Mesh {

	public:
		Mesh() {}
		Mesh(MaterialContainer& material) : material(&material) {}

		virtual void draw() const = 0;

		virtual unsigned int getIndicesCount() const = 0;
		virtual unsigned int getVerticesCount() const = 0;

		//Returns vertex index at index i or 0 if i is not present
		virtual unsigned int getIndex(unsigned int i) const = 0;
		virtual const glm::vec3& getVertexPosition(unsigned int i) const = 0;

		MaterialContainer& getMaterialContainer() const;

	protected:
		MaterialContainer* material;

	};


	//Mesh container class for static geometry
	class StaticMesh : public Mesh {

	public:
		StaticMesh() : Mesh() {}
		StaticMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, MaterialContainer& material);

		virtual void draw() const;

		virtual unsigned int getIndicesCount() const;
		virtual unsigned int getVerticesCount() const;

		//Returns vertex at index i or emptry Vertex if i is not present
		virtual const Vertex& getVertex(unsigned int i) const;
		virtual unsigned int getIndex(unsigned int i) const;
		virtual const glm::vec3& getVertexPosition(unsigned int i) const;

	private:
		unsigned int vao, vbo, ebo;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		void init();
	};


	//Mesh container for skinable/animatable geometry
	class SkinnedMesh : public Mesh {

	public:
		SkinnedMesh(std::vector<SkinnedVertex>& vertices, std::vector<unsigned int>& indices, 
			std::map<std::string, MeshBoneData>& bones, MaterialContainer& material);

		virtual void draw() const;

		void updateMeshBoneData(const Skeleton& skeleton);

		virtual unsigned int getIndicesCount() const;
		virtual unsigned int getVerticesCount() const;

		//Returns vertex at index i or emptry Vertex if i is not present
		virtual const SkinnedVertex& getVertex(unsigned int i) const;
		virtual unsigned int getIndex(unsigned int i) const;
		virtual const glm::vec3& getVertexPosition(unsigned int i) const;

		unsigned int getBoneID(std::string name) const;
		void setBoneID(std::string name, unsigned int id);

		std::map<std::string, MeshBoneData>::iterator bonesBegin();
		std::map<std::string, MeshBoneData>::iterator bonesEnd();

		std::map<std::string, MeshBoneData>::const_iterator bonesBeginConst() const;
		std::map<std::string, MeshBoneData>::const_iterator bonesEndBegin() const;

	private:
		unsigned int vao, vbo, ebo;
		std::vector<SkinnedVertex> vertices;
		std::vector<unsigned int> indices;
		std::map<std::string, MeshBoneData> bones;

		void init();
	};
}

#endif
