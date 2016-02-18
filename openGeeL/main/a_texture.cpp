#include "a_texture.h"

namespace {

class TextureTestObject : public RenderObject {

public:

	Shader* shader;
	GLuint VBO, VAO, EBO;
	GLuint texture1, texture2;
	LayeredTexture texmex;

	TextureTestObject() {}

	virtual void init() {
		shader = new Shader("renderer/shaders/simpleTex.vert", "renderer/shaders/simpleTex.frag");

		//Actual geometry
		GLfloat vertices[] = {
			// Positions          // Colors           // Texture Coords
			0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
			0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left 
		};

		GLuint indices[] = {
			0, 1, 3,  // First Triangle
			1, 2, 3   // Second Triangle
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		// TexCoord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		SimpleTexture tex1 = SimpleTexture("tex", "data/images/container.jpg", GL_REPEAT, GL_LINEAR);
		SimpleTexture tex2 = SimpleTexture("tex2", "data/images/awesomeface.png", GL_REPEAT, GL_LINEAR);

		texmex = LayeredTexture();
		texmex.addTexture(tex1);
		texmex.addTexture(tex2);
	}

	virtual void update() {

		GLfloat timeValue = glfwGetTime();
		GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
		GLint mixLocation = glGetUniformLocation(shader->program, "mixValue");
		glUniform1f(mixLocation, greenValue);

		texmex.draw();
		shader->use();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}

	virtual void quit() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
};

}


void a_texture() {

	TextureTestObject* testObj = new TextureTestObject();

	RenderWindow* window = new RenderWindow("TextureTest", 800, 600);
	InputManager* manager = new InputManager();
	Renderer renderer = Renderer(window, manager);
	renderer.addObject(testObj);
	renderer.render();

	delete testObj;
	delete window;
	delete manager;
}