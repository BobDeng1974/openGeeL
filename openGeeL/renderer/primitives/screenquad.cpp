#define GLEW_STATIC
#include <glew.h>
#include "screenquad.h"

namespace geeL {

	void ScreenQuad::init() {
		if (initialized()) return;

		GLfloat quadVertices[] = {
			//Positions     //Texture Coords
			-1.f, 1.f, 0.f,  0.f, 1.f,
			-1.f, -1.f, 0.f, 0.f, 0.f,
			1.f, 1.f, 0.f,   1.f, 1.f,
			1.f, -1.f, 0.f,  1.f, 0.f,
		};

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);
	}

	void ScreenQuad::draw() const {
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	ScreenQuad SCREENQUAD;
	ScreenQuad& ScreenQuad::get() {
		return SCREENQUAD;
	}

}