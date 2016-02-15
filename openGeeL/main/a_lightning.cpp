#include "a_lightning.h"
#include <iostream>
#include "../renderer/cameras/camera.h"
#include "../renderer/cameras/perspectivecamera.h"

#define pi 3.141592f

namespace {

	class LightTestObject : public RenderObject {

	public:

		Shader* shader;
		Shader* lamp;
		GLuint VBO, VAO, containerVAO, lightVAO;
		GLuint texture1, texture2;
		SimpleTexture texmex;

		PerspectiveCamera camera;

		LightTestObject() {}

		virtual void init() {


			//glfwSetInputMode(window->glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			shader = new Shader("renderer/shaders/lighting.vert", "renderer/shaders/lighting.frag");
			lamp = new Shader("renderer/shaders/lighting.vert", "renderer/shaders/lamp.frag");

			camera = PerspectiveCamera(inputManager, glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -pi / 2, 0.f, 5.f, 0.5f,
				45.f, window->width, window->height, 0.1f, 100.f);

			GLfloat vertices[] = {
				-0.5f, -0.5f, -0.5f,
				0.5f, -0.5f, -0.5f,
				0.5f,  0.5f, -0.5f,
				0.5f,  0.5f, -0.5f,
				-0.5f,  0.5f, -0.5f,
				-0.5f, -0.5f, -0.5f,

				-0.5f, -0.5f,  0.5f,
				0.5f, -0.5f,  0.5f,
				0.5f,  0.5f,  0.5f,
				0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f,  0.5f,
				-0.5f, -0.5f,  0.5f,

				-0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f, -0.5f,
				-0.5f, -0.5f, -0.5f,
				-0.5f, -0.5f, -0.5f,
				-0.5f, -0.5f,  0.5f,
				-0.5f,  0.5f,  0.5f,

				0.5f,  0.5f,  0.5f,
				0.5f,  0.5f, -0.5f,
				0.5f, -0.5f, -0.5f,
				0.5f, -0.5f, -0.5f,
				0.5f, -0.5f,  0.5f,
				0.5f,  0.5f,  0.5f,

				-0.5f, -0.5f, -0.5f,
				0.5f, -0.5f, -0.5f,
				0.5f, -0.5f,  0.5f,
				0.5f, -0.5f,  0.5f,
				-0.5f, -0.5f,  0.5f,
				-0.5f, -0.5f, -0.5f,

				-0.5f,  0.5f, -0.5f,
				0.5f,  0.5f, -0.5f,
				0.5f,  0.5f,  0.5f,
				0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f,  0.5f,
				-0.5f,  0.5f, -0.5f
			};

			

			glGenVertexArrays(1, &containerVAO);
			glGenBuffers(1, &VBO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindVertexArray(containerVAO);
			// Position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);

			// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
			glGenVertexArrays(1, &lightVAO);
			glBindVertexArray(lightVAO);
			// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// Set the vertex attributes (only position data for the lamp))
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);

			texmex = SimpleTexture("tex", "data/images/container.jpg", GL_REPEAT, GL_LINEAR);
		}

		glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
		virtual void update() {

			camera.update();

			texmex.draw(shader->program, 0);
			shader->use();


			// Use cooresponding shader when setting uniforms/drawing objects
			
			GLint objectColorLoc = glGetUniformLocation(shader->program, "objectColor");
			GLint lightColorLoc = glGetUniformLocation(shader->program, "lightColor");
			glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
			glUniform3f(lightColorLoc, 1.0f, 0.5f, 1.0f);



			glm::mat4 view;
			glm::mat4 projection;
			GLint modelLoc = glGetUniformLocation(shader->program, "model");
			GLint viewLoc = glGetUniformLocation(shader->program, "view");
			GLint projLoc = glGetUniformLocation(shader->program, "projection");

			view = camera.viewMatrix();
			projection = camera.projectionMatrix();

			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			// Draw the container (using container's vertex attributes)
			glBindVertexArray(containerVAO);
			glm::mat4 model;
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			// Also draw the lamp object, again binding the appropriate shader
			lamp->use();
			// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
			modelLoc = glGetUniformLocation(lamp->program, "model");
			viewLoc = glGetUniformLocation(lamp->program, "view");
			projLoc = glGetUniformLocation(lamp->program, "projection");
			// Set matrices
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			model = glm::mat4();
			model = glm::translate(model, lightPos);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			// Draw the light object (using light's vertex attributes)
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}

		virtual void quit() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}
	};

}


void a_lighting() {

	LightTestObject* testObj = new LightTestObject();

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