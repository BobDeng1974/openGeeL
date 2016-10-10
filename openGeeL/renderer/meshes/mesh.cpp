#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "mesh.h"
#include "../cameras/camera.h"
#include "../materials/material.h"
#include "../lighting/lightmanager.h"

namespace geeL {

	Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, Material& material)
		: vertices(vertices), indices(indices), material(material) {
	
		init();
	}

	void Mesh::init() {
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

		glBindVertexArray(0);

		material.bindTextures();
	}

	void Mesh::draw(bool shade) const {
		draw(material, shade);
	}

	void Mesh::draw(Material& customMaterial) const {
		draw(customMaterial, true);
	}

	void Mesh::draw(Material& customMaterial, bool shade) const {
		if (shade) {
			customMaterial.bindTextures();
			customMaterial.bind();
		}
			

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	
}