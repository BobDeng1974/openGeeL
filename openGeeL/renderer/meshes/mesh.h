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

	class Bone;
	class MaterialContainer;
	class RenderShader;
	class Shader;
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


	//Stores how single bone needs to be processed by mesh
	struct MeshBone {
		Bone* bone = nullptr;
		unsigned int id;
		glm::mat4 offsetMatrix;

		glm::mat4 transform;

	};


	//Base class for all mesh containers
	class Mesh {

	public:
		Mesh() {}
		Mesh(const std::string& name, MaterialContainer& material) 
			: name(name), material(&material) {}

		virtual void draw(const Shader& shader) const = 0;

		virtual size_t getIndicesCount() const = 0;
		virtual size_t getVerticesCount() const = 0;

		//Returns vertex index at index i or clamps index to high
		virtual unsigned int getIndex(size_t i) const = 0;
		virtual const glm::vec3& getVertexPosition(size_t i) const = 0;

		const std::string& getName() const;
		MaterialContainer& getMaterialContainer() const;

	protected:
		std::string name;
		MaterialContainer* material;

	};


	//Mesh container class for static geometry
	class StaticMesh : public Mesh {

	public:
		StaticMesh() : Mesh() {}
		StaticMesh(const std::string& name, 
			std::vector<Vertex>& vertices, 
			std::vector<unsigned int>& indices, 
			MaterialContainer& material);

		virtual void draw(const Shader& shader) const;

		virtual size_t getIndicesCount() const;
		virtual size_t getVerticesCount() const;

		//Returns vertex at index i or emptry Vertex if i is not present
		virtual const Vertex& getVertex(size_t i) const;
		virtual unsigned int getIndex(size_t i) const;
		virtual const glm::vec3& getVertexPosition(size_t i) const;

	private:
		unsigned int vao, vbo, ebo;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		void init();
	};


	//Mesh container for skinable/animatable geometry
	class SkinnedMesh : public Mesh {

	public:
		SkinnedMesh(const std::string& name, 
			std::vector<SkinnedVertex>& vertices, 
			std::vector<unsigned int>& indices,
			std::map<std::string, MeshBone>& bones,
			MaterialContainer& material);

		
		virtual void draw(const Shader& shader) const;

		virtual size_t getIndicesCount() const;
		virtual size_t getVerticesCount() const;

		//Returns vertex at index i or emptry Vertex if i is not present
		virtual const SkinnedVertex& getVertex(size_t i) const;
		virtual unsigned int getIndex(size_t i) const;
		virtual const glm::vec3& getVertexPosition(size_t i) const;

		std::map<std::string, MeshBone>::iterator bonesBegin();
		std::map<std::string, MeshBone>::iterator bonesEnd();

		std::map<std::string, MeshBone>::const_iterator bonesBeginConst() const;
		std::map<std::string, MeshBone>::const_iterator bonesEndBegin() const;

	private:
		unsigned int vao, vbo, ebo;
		std::vector<SkinnedVertex> vertices;
		std::vector<unsigned int> indices;
		std::map<std::string, MeshBone> bones;

		//Update mesh bone data into given shader
		void updateBones(const Shader& shader) const;

		void init();
	};




	inline const std::string& Mesh::getName() const {
		return name;
	}



}

#endif
