#ifndef MESH_H
#define MESH_H

#include <vec2.hpp>
#include <vec3.hpp>
#include <string>
#include <vector>

using namespace std;
using glm::vec3;
using glm::vec2;

typedef unsigned int GLuint;

namespace geeL {

class Camera;
class LightManager;
class Material;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	vec2 texCoords;
};


class Mesh {

public:
	Material& material;
	vector<Vertex> vertices;
	vector<GLuint> indices;

	Mesh(vector<Vertex> vertices, vector<GLuint> indices, Material& material);

	void init();
	void draw();
	void draw(Material& customMaterial);

private:
	GLuint vao, vbo, ebo;

};

}

#endif
