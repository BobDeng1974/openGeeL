#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include <glfw3.h>
#include "mesh.h"
#include "materials/defaultmaterial.h"
#include "materials/material.h"
#include "animation/bone.h"
#include "animation/skeleton.h"
#include "shader/rendershader.h"

using namespace std;

namespace geeL {
	
	Mesh::Mesh(const std::string& name, std::shared_ptr<MaterialContainer> material)
		: name(name)
		, material(material) {}

	Mesh::Mesh(Mesh&& other)
		: name(std::move(other.name))
		, material(other.material) {

		other.material.reset();
	}

	Mesh& Mesh::operator=(Mesh&& other) {
		if (this != &other) {
			name = std::move(other.name);
			material = other.material;

			other.material.reset();
		}

		return *this;
	}


	MaterialContainer& Mesh::getMaterialContainer() const {
		return *material;
	}

	template<typename VertexType>
	void GenericMesh<VertexType>::draw(const Shader& shader) const {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, int(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}



	StaticMesh::StaticMesh() 
		: GenericMesh<Vertex>() {}

	StaticMesh::StaticMesh(const std::string& name, 
		std::vector<Vertex>& vertices, 
		std::vector<unsigned int>& indices, 
		std::shared_ptr<MaterialContainer> material)
		: GenericMesh<Vertex>(name, 
			std::move(vertices), 
			std::move(indices), 
			material) {

		init();
	}

	StaticMesh::StaticMesh(StaticMesh&& other)
		: GenericMesh<Vertex>(std::move(other)) {

		init();
	}

	StaticMesh& StaticMesh::operator=(StaticMesh&& other) {
		if (this != &other)
			GenericMesh<Vertex>::operator=(std::move(other));

		return *this;
	}

	void StaticMesh::init() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bitangent));

		glBindVertexArray(0);
	}



	SkinnedMesh::SkinnedMesh(const std::string& name, 
		vector<SkinnedVertex>& vertices, 
		vector<unsigned int>& indices,
		std::map<std::string, MeshBone>& bones,
		std::shared_ptr<MaterialContainer> material)
			: GenericMesh<SkinnedVertex>(name
				, std::move(vertices)
				, std::move(indices)
				, material)
			, bones(std::move(bones)) {

		init();
	}

	SkinnedMesh::SkinnedMesh(SkinnedMesh&& other)
		: GenericMesh<SkinnedVertex>(std::move(other))
		, bones(std::move(other.bones)) {

		init();
	}

	SkinnedMesh& SkinnedMesh::operator=(SkinnedMesh&& other) {
		if (this != &other) {
			GenericMesh<SkinnedVertex>::operator=(std::move(other));
			bones = std::move(other.bones);

			init();
		}

		return *this;
	}

	void SkinnedMesh::init() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SkinnedVertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, texCoords));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, tangent));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, bitangent));

		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, BONECOUNT, GL_INT, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, IDs));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, BONECOUNT, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, weights));

		glBindVertexArray(0);
	}

	void SkinnedMesh::updateBones(const Shader& shader, const Skeleton& skeleton) const {
		for (auto it(bones.begin()); it != bones.end(); it++) {
			const MeshBone& data = it->second;
			const Bone* bone = skeleton.getBone(it->first);
			mat4 finalTransform = bone->getMatrix() * data.offsetMatrix;

			shader.bind<glm::mat4>("bones[" + std::to_string(data.id) + "]", finalTransform);
		}
	}

	
}