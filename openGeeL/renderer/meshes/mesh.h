#ifndef MESH_H
#define MESH_H

#include <vec2.hpp>
#include <vec3.hpp>
#include <string>
#include <vector>

using glm::vec3;
using glm::vec2;

typedef unsigned int GLuint;

namespace geeL {

class DefaultMaterial;
class Material;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};


class Mesh {

public:
	DefaultMaterial& material;
	
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, DefaultMaterial& material);

	void init();
	void draw(bool shade = true) const;
	void draw(Material& customMaterial) const;

private:
	GLuint vao, vbo, ebo;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	void draw(Material& customMaterial, bool shade) const;

};

}

#endif
