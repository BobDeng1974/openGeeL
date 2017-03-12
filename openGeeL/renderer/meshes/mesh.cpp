#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "mesh.h"
#include "../materials/defaultmaterial.h"
#include "../materials/material.h"
#include <iostream>

using namespace std;
using namespace glm;

namespace geeL {


	void Mesh::draw(Material& customMaterial) const {
		customMaterial.bindTextures();
		customMaterial.bind();

		draw();
	}


	MaterialContainer& Mesh::getMaterialContainer() const {
		return *material;
	}



	StaticMesh::StaticMesh(vector<Vertex>& vertices, vector<unsigned int>& indices, MaterialContainer& material)
		: Mesh(material), vertices(vertices), indices(indices) {
	
		init();
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

	void StaticMesh::draw() const {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	unsigned int StaticMesh::getIndicesCount() const {
		return indices.size();
	}

	unsigned int StaticMesh::getVerticesCount() const {
		return vertices.size();
	}

	const Vertex& StaticMesh::getVertex(unsigned int i) const {
		if (i < vertices.size())
			return vertices[i];

		return Vertex();
	}

	unsigned int StaticMesh::getIndex(unsigned int i) const {
		if (i < indices.size())
			return indices[i];

		return 0;
	}

	const glm::vec3& StaticMesh::getVertexPosition(unsigned int i) const {
		if (i < vertices.size())
			return vertices[i].position;

		return glm::vec3();
	}




	SkinnedMesh::SkinnedMesh(vector<SkinnedVertex>& vertices, vector<unsigned int>& indices, 
		std::map<std::string, MeshBoneData>& bones, MaterialContainer& material)
			: Mesh(material), vertices(vertices), indices(indices), bones(bones) {

		init();
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
		glVertexAttribIPointer(5, BONECOUNT, GL_INT, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, bones));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, BONECOUNT, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex),
			(GLvoid*)(offsetof(SkinnedVertex, bones) + offsetof(VertexBoneData, weights)));

		glBindVertexArray(0);
	}

	void SkinnedMesh::draw() const {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	unsigned int SkinnedMesh::getIndicesCount() const {
		return indices.size();
	}

	unsigned int SkinnedMesh::getVerticesCount() const {
		return vertices.size();
	}

	const SkinnedVertex& SkinnedMesh::getVertex(unsigned int i) const {
		if (i < vertices.size())
			return vertices[i];

		return SkinnedVertex();
	}

	unsigned int SkinnedMesh::getIndex(unsigned int i) const {
		if (i < indices.size())
			return indices[i];

		return 0;
	}

	const glm::vec3& SkinnedMesh::getVertexPosition(unsigned int i) const {
		if (i < vertices.size())
			return vertices[i].position;

		return vec3();
	}
}