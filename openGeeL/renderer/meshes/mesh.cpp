#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "mesh.h"
#include "materials/defaultmaterial.h"
#include "materials/material.h"
#include "animation/bone.h"
#include "animation/skeleton.h"
#include "shader/rendershader.h"

using namespace std;

namespace geeL {

	MaterialContainer& Mesh::getMaterialContainer() const {
		return *material;
	}


	StaticMesh::StaticMesh(const std::string& name, 
		vector<Vertex>& vertices, 
		vector<unsigned int>& indices, 
		MaterialContainer& material)
			: Mesh(name, material)
			, vertices(std::move(vertices))
			, indices(std::move(indices)) {
	
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

	void StaticMesh::draw(const Shader& shader) const {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, int(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	size_t StaticMesh::getIndicesCount() const {
		return indices.size();
	}

	size_t StaticMesh::getVerticesCount() const {
		return vertices.size();
	}

	const Vertex& StaticMesh::getVertex(size_t i) const {
		if (i >= vertices.size())
			i = vertices.size() - 1;

		return vertices[i];
	}

	unsigned int StaticMesh::getIndex(size_t i) const {
		if (i >= vertices.size())
			i = vertices.size() - 1;
			
		return indices[i];
	}

	const glm::vec3& StaticMesh::getVertexPosition(size_t i) const {
		if (i >= vertices.size())
			i = vertices.size() - 1;

		return vertices[i].position;
	}



	SkinnedMesh::SkinnedMesh(const std::string& name, 
		vector<SkinnedVertex>& vertices, 
		vector<unsigned int>& indices,
		std::map<std::string, MeshBone>& bones,
		MaterialContainer& material)
			: Mesh(name, material)
			, vertices(std::move(vertices))
			, indices(std::move(indices))
			, bones(std::move(bones)) {

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
		glVertexAttribIPointer(5, BONECOUNT, GL_INT, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, IDs));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, BONECOUNT, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (GLvoid*)offsetof(SkinnedVertex, weights));

		glBindVertexArray(0);
	}

	void SkinnedMesh::updateBones(const Shader& shader) const {
		for (auto it(bones.begin()); it != bones.end(); it++) {
			const MeshBone& data = it->second;
			Bone* bone = data.bone;
			mat4 finalTransform = bone->getMatrix() * data.offsetMatrix;

			shader.bind<glm::mat4>("bones[" + std::to_string(data.id) + "]", finalTransform);
		}
	}

	void SkinnedMesh::updateBones(const Shader& shader, const Skeleton& skeleton) const {
		for (auto it(bones.begin()); it != bones.end(); it++) {
			const MeshBone& data = it->second;
			const Bone* bone = skeleton.getBone(it->first);
			mat4 finalTransform = bone->getMatrix() * data.offsetMatrix;

			shader.bind<glm::mat4>("bones[" + std::to_string(data.id) + "]", finalTransform);
		}
	}

	void SkinnedMesh::draw(const Shader& shader) const {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, int(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	size_t SkinnedMesh::getIndicesCount() const {
		return unsigned int(indices.size());
	}

	size_t SkinnedMesh::getVerticesCount() const {
		return unsigned int(vertices.size());
	}

	const SkinnedVertex& SkinnedMesh::getVertex(size_t i) const {
		if (i >= vertices.size())
			i = vertices.size() - 1;

		return vertices[i];
	}

	unsigned int SkinnedMesh::getIndex(size_t i) const {
		if (i >= vertices.size())
			i = vertices.size() - 1;

		return indices[i];
	}

	const glm::vec3& SkinnedMesh::getVertexPosition(size_t i) const {
		if (i >= vertices.size())
			i = vertices.size() - 1;

		return vertices[i].position;
	}

	std::map<std::string, MeshBone>::iterator  SkinnedMesh::bonesBegin() {
		return bones.begin();
	}

	std::map<std::string, MeshBone>::iterator  SkinnedMesh::bonesEnd() {
		return bones.end();
	}

	std::map<std::string, MeshBone>::const_iterator  SkinnedMesh::bonesBeginConst() const {
		return bones.begin();
	}

	std::map<std::string, MeshBone>::const_iterator  SkinnedMesh::bonesEndBegin() const {
		return bones.end();
	}

}