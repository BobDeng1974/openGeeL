#include "a_lightning.h"
#include <iostream>
#include "../renderer/cameras/camera.h"
#include "../renderer/cameras/perspectivecamera.h"
#include "../renderer/lighting/light.h"
#include "../renderer/lighting/lightmanager.h"
#include "../renderer/lighting/pointlight.h"

#define pi 3.141592f

namespace {

	class LightTestObject : public RenderObject {

	public:

		Shader* shader;
		Shader* lamp;
		GLuint VBO, VAO, containerVAO, lightVAO;
		GLuint texture1, texture2;
		SimpleTexture texmex;
		SimpleTexture spectex;
		LayeredTexture laytex;
		LightManager manager;
		
		glm::vec3 cubePositions[10];

		PerspectiveCamera camera;

		LightTestObject() {}

		virtual void init() {


			//glfwSetInputMode(window->glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			shader = new Shader("renderer/shaders/lighting.vert", "renderer/shaders/lighting.frag");
			lamp = new Shader("renderer/shaders/lighting.vert", "renderer/shaders/lamp.frag");

			camera = PerspectiveCamera(inputManager, glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -pi / 2, 0.f, 5.f, 0.5f,
				45.f, window->width, window->height, 0.1f, 100.f);

			GLfloat vertices[] = {
				// Positions          // Normals           // Texture Coords
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
				0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

				0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
				0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
				0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
				0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
				0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
				0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
				-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
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

			glGenVertexArrays(1, &containerVAO);
			glGenBuffers(1, &VBO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindVertexArray(containerVAO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			glBindVertexArray(0);

			// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
			glGenVertexArrays(1, &lightVAO);
			glBindVertexArray(lightVAO);
			// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// Set the vertex attributes (only position data for the lamp))
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);


			texmex = SimpleTexture("material.diffuse", "data/images/container2.png", GL_REPEAT, GL_LINEAR);
			spectex = SimpleTexture("material.specular", "data/images/container2_specular.png", GL_REPEAT, GL_LINEAR);
			laytex = LayeredTexture();

			shader->use();
			laytex.addTexture(texmex);
			laytex.addTexture(spectex);
			laytex.bind(shader);


			PointLight* light1 = new PointLight(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
			PointLight* light2 = new PointLight(glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.3f, 0.3f, 0.9f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f));

			manager = LightManager();
			//manager.addLight(light1);
			manager.addLight(light2);
			manager.addReceiver(shader);

			manager.bind();


		}

		glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
		virtual void update() {

			camera.update();
			shader->use();

			GLint cameraPositionLoc = glGetUniformLocation(shader->program, "viewerPosition");
			glUniform3f(cameraPositionLoc, camera.position.x, camera.position.y, camera.position.z);

			GLint matShineLoc = glGetUniformLocation(shader->program, "material.shininess");
			glUniform1f(matShineLoc, 64.0f);

			GLint lightAmbientLoc = glGetUniformLocation(shader->program, "lights[0].ambient");
			GLint lightDiffuseLoc = glGetUniformLocation(shader->program, "lights[0].diffuse");
			GLint lightSpecularLoc = glGetUniformLocation(shader->program, "lights[0].specular");
			GLint lightPositionLoc = glGetUniformLocation(shader->program, "lights[0].position");

			glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
			glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f); 
			glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
			glUniform3f(lightPositionLoc, lightPos.x, lightPos.y, lightPos.z);

			glUniform1f(glGetUniformLocation(shader->program, "lights[0].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(shader->program, "lights[0].linear"), 0.09);
			glUniform1f(glGetUniformLocation(shader->program, "lights[0].quadratic"), 0.032);


			laytex.draw();

			glm::mat4 view;
			glm::mat4 projection;
			GLint modelLoc = glGetUniformLocation(shader->program, "model");
			GLint viewLoc = glGetUniformLocation(shader->program, "view");
			GLint projLoc = glGetUniformLocation(shader->program, "projection");

			view = camera.viewMatrix();
			projection = camera.projectionMatrix();

			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glm::mat4 model;
			glBindVertexArray(containerVAO);
			for (GLuint i = 0; i < 10; i++)
			{
				model = glm::mat4();
				model = glm::translate(model, cubePositions[i]);
				GLfloat angle = 20.0f * i;
				model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
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