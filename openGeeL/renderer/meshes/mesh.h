#ifndef MESH_H
#define MESH_H

#include <vec2.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include <string>
#include <vector>
#include <map>
#include "animation/bone.h"

#define BONECOUNT 4

namespace geeL {

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
	struct SkinnedVertex : public Vertex {
		unsigned int IDs[BONECOUNT];
		float weights[BONECOUNT];

		SkinnedVertex();
		void addBone(unsigned int id, float weight);

	};


	//Stores how single bone needs to be processed by mesh
	struct MeshBone {
		unsigned int id;
		glm::mat4 offsetMatrix;
	};


	//Base class for all mesh containers
	class Mesh {

	public:
		Mesh() {}
		Mesh(const std::string& name, MaterialContainer& material) 
			: name(name), material(&material) {}

		Mesh(Mesh&& other);
		Mesh& operator=(Mesh&& other);


		virtual void draw(const Shader& shader) const = 0;

		virtual size_t getIndicesCount() const = 0;
		virtual size_t getVerticesCount() const = 0;

		virtual unsigned int getIndex(size_t i) const = 0;
		virtual const Vertex& getVertex(size_t i) const = 0;

		const std::string& getName() const;
		MaterialContainer& getMaterialContainer() const;

	protected:
		std::string name;
		MaterialContainer* material;

	};

	template<typename VertexType>
	class GenericMesh : public Mesh {

	public:
		GenericMesh();
		GenericMesh(const std::string& name,
			std::vector<VertexType>&& vertices,
			std::vector<unsigned int>&& indices,
			MaterialContainer& material);

		GenericMesh(GenericMesh<VertexType>&& other);
		GenericMesh<VertexType>& operator=(GenericMesh<VertexType>&& other);

		virtual void draw(const Shader& shader) const;

		virtual size_t getIndicesCount() const;
		virtual size_t getVerticesCount() const;

		//Returns vertex at index i or emptry Vertex if i is not present
		virtual const Vertex& getVertex(size_t i) const;
		virtual unsigned int getIndex(size_t i) const;

	protected:
		unsigned int vao, vbo, ebo;
		std::vector<VertexType> vertices;
		std::vector<unsigned int> indices;

	};


	//Mesh container class for static geometry
	class StaticMesh : public GenericMesh<Vertex> {

	public:
		StaticMesh();
		StaticMesh(const std::string& name,
			std::vector<Vertex>& vertices,
			std::vector<unsigned int>& indices,
			MaterialContainer & material);

		StaticMesh(StaticMesh&& other);
		StaticMesh& operator=(StaticMesh&& other);

	private:
		virtual void init();
		
	};


	//Mesh container for skinable/animatable geometry
	class SkinnedMesh : public GenericMesh<SkinnedVertex> {

	public:
		SkinnedMesh(const std::string& name, 
			std::vector<SkinnedVertex>& vertices, 
			std::vector<unsigned int>& indices,
			std::map<std::string, MeshBone>& bones,
			MaterialContainer& material);

		SkinnedMesh(SkinnedMesh&& other);
		SkinnedMesh& operator=(SkinnedMesh&& other);


		//Update mesh bone data into given shader
		void updateBones(const Shader& shader, const Skeleton& skeleton) const;
		
	private:
		std::map<std::string, MeshBone> bones;

		void init();
	};





	inline SkinnedVertex::SkinnedVertex() {
		for (unsigned int i = 0; i < BONECOUNT; i++) {
			IDs[i] = 0;
			weights[i] = 0.f;
		}
	}

	inline void SkinnedVertex::addBone(unsigned int id, float weight) {
		//Find unused spot in arrays
		for (unsigned int i = 0; i < BONECOUNT; i++) {
			if (weights[i] == 0.f) {
				IDs[i] = id;
				weights[i] = weight;
				return;
			}
		}
	}


	inline const std::string& Mesh::getName() const {
		return name;
	}


	template<typename VertexType>
	GenericMesh<VertexType>::GenericMesh()
		: Mesh() {}

	template<typename VertexType>
	GenericMesh<VertexType>::GenericMesh(const std::string& name, 
		std::vector<VertexType>&& vertices, 
		std::vector<unsigned int>&& indices, 
		MaterialContainer & material)
			: Mesh(name, material)
			, vertices(std::move(vertices))
			, indices(std::move(indices)) {}

	template<typename VertexType>
	inline GenericMesh<VertexType>::GenericMesh(GenericMesh<VertexType>&& other)
		: Mesh(std::move(other))
		, vao(other.vao) 
		, vbo(other.vbo) 
		, ebo(other.ebo)
		, vertices(std::move(other.vertices)) 
		, indices(std::move(other.indices)) {

		other.vao = 0;
		other.vbo = 0;
		other.ebo = 0;
	}

	template<typename VertexType>
	inline GenericMesh<VertexType>& GenericMesh<VertexType>::operator=(GenericMesh<VertexType>&& other) {
		if (this != &other) {
			Mesh::operator=(std::move(other));

			vao = other.vao;
			vbo = other.vbo;
			ebo = other.ebo;
			vertices = std::move(other.vertices);
			indices = std::move(other.indices);

			other.vao = 0;
			other.vbo = 0;
			other.ebo = 0;
		}

		return *this;
	}


	template<typename VertexType>
	inline size_t GenericMesh<VertexType>::getIndicesCount() const {
		return indices.size();
	}

	template<typename VertexType>
	inline size_t GenericMesh<VertexType>::getVerticesCount() const {
		return vertices.size();
	}

	template<typename VertexType>
	inline const Vertex& GenericMesh<VertexType>::getVertex(size_t i) const {
		if (i >= vertices.size())
			i = vertices.size() - 1;

		return vertices[i];
	}

	template<typename VertexType>
	inline unsigned int GenericMesh<VertexType>::getIndex(size_t i) const {
		if (i >= vertices.size())
			i = vertices.size() - 1;

		return indices[i];
	}

}

#endif
