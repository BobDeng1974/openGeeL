#include "a_cameras.h"
#include <iostream>
#include "../renderer/cameras/camera.h"
#include "../renderer/cameras/perspectivecamera.h"

#define pi 3.141592f

namespace {

	class CameraTestObject : public RenderObject {

	public:

		Shader* shader;
		GLuint VBO, VAO, EBO;
		GLuint texture1, texture2;
		SimpleTexture texmex;
		glm::vec3 cubePositions[10];

		PerspectiveCamera camera;

		CameraTestObject() {}

		virtual void init() {


			//glfwSetInputMode(window->glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			shader = new Shader("renderer/shaders/transform.vert", "renderer/shaders/simpleTex.frag");

			camera = PerspectiveCamera(inputManager, glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -pi / 2, 0.f, 5.f, 0.5f, 
				45.f, window->width, window->height, 0.1f, 100.f);

			//Actual geometry
			GLfloat vertices[] = {

				-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
				0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
				0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
				0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
				0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
				-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

				-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

				0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
				0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
				0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

				-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
				0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
				-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
			};

			cubePositions[0] = glm::vec3(0.0f, 0.0f, 0.0f);
			cubePositions[1] = glm::vec3(2.0f, 5.0f, -15.0f);
			cubePositions[2] = glm::vec3(-1.5f, -2.2f, -2.5f);
			cubePositions[3] = glm::vec3(-3.8f, -2.0f, -12.3f);
			cubePositions[4] = glm::vec3(2.4f, -0.4f, -3.5f);
			cubePositions[5] = glm::vec3(-1.7f, 3.0f, -7.5f);
			cubePositions[6] = glm::vec3(1.3f, -2.0f, -2.5f);
			cubePositions[7] = glm::vec3(1.5f, 2.0f, -2.5f);
			cubePositions[8] = glm::vec3(1.5f, 0.2f, -1.5f);
			cubePositions[9] = glm::vec3(-1.3f, 1.0f, -1.5f);

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			//Positions
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			//Texture coordinates
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);

			//glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			texmex = SimpleTexture("tex", "data/images/container.jpg", GL_REPEAT, GL_LINEAR);
		}

		virtual void update() {

			camera.update();

			texmex.draw(shader->program, 0);
			shader->use();

			glm::mat4 view;
			glm::mat4 projection;

			GLint modelLoc = glGetUniformLocation(shader->program, "model");
			GLint viewLoc = glGetUniformLocation(shader->program, "view");
			GLint projLoc = glGetUniformLocation(shader->program, "projection");

			view = camera.viewMatrix();
			projection = camera.projectionMatrix();

			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(VAO);
			for (int i = 0; i < 10; i++) {
				glm::mat4 model;
				model = glm::translate(model, cubePositions[i]);
				GLfloat angle = 20.0f * (i + 1) * (GLfloat)glfwGetTime();
				model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 1.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			glBindVertexArray(0);
		}

		virtual void quit() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
		}
	};

}


void a_cameras() {

	CameraTestObject* testObj = new CameraTestObject();

	RenderWindow* window = new RenderWindow("CameraTest", 1500, 850, true);
	InputManager* manager = new InputManager();

	manager->defineButton("Forward", GLFW_KEY_W);
	manager->defineButton("Forward", GLFW_KEY_A);

	Renderer renderer = Renderer(window, manager);
	renderer.addObject(testObj);
	renderer.render();

	delete testObj;
	delete window;
	delete manager;
}