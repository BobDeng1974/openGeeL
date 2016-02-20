#include "a_lightning.h"
#include <iostream>
#include "../renderer/cameras/camera.h"
#include "../renderer/cameras/perspectivecamera.h"
#include "../renderer/lighting/light.h"
#include "../renderer/lighting/lightmanager.h"
#include "../renderer/lighting/pointlight.h"
#include "../renderer/shader/shadermanager.h"
#include "../renderer/transform.h"

#define pi 3.141592f

namespace {

	class LightTestObject : public RenderObject {

	public:

		const Shader* shader;
		const Shader* lamp;
		GLuint VBO, VAO, containerVAO, lightVAO;
		GLuint texture1, texture2;
		SimpleTexture texmex;
		SimpleTexture spectex;
		LayeredTexture laytex;
		LightManager manager;
		
		glm::vec3 cubePositions[10];

		//Camera* camera;

		LightTestObject(const RenderWindow* window, ShaderManager* shaderManager) 
			: RenderObject(window, shaderManager) {}

		virtual void init(const Camera* const camera) {


			//glfwSetInputMode(window->glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			shader = shaderManager->getShader("Standard");
			lamp = shaderManager->getShader("Lamp");

			geeL::Transform* transform = new geeL::Transform(glm::vec3(0.0f, 0.0f, 3.0f), vec3(0.f, -90.f, 0.f), vec3(1.f, 1.f, 1.f));

			//camera = window->getCamera();

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
			laytex.bind(*shader);


			PointLight* light1 = new PointLight(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.2f, 0.2f, 0.2f));
			PointLight* light2 = new PointLight(glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.3f, 0.3f, 0.9f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f));

			manager = LightManager();
			manager.addLight(light1);
			//manager.addLight(light2);
			manager.addReceiver(*shader);

			manager.bind();


		}

		struct shader_data_t {
			vec3 position;

			vec3 ambient;
			vec3 diffuse;
			vec3 specular;

			float constant;
			float linear;
			float quadratic;
		};

		glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
		virtual void update(const Camera* const camera) {

			GLuint program = shader->program;

			shader->use();

			vec3 cameraposition = camera->transform->position;

			GLint cameraPositionLoc = glGetUniformLocation(program, "viewerPosition");
			glUniform3f(cameraPositionLoc, cameraposition.x, cameraposition.y, cameraposition.z);

			GLint matShineLoc = glGetUniformLocation(program, "material.shininess");
			glUniform1f(matShineLoc, 64.0f);

			shader_data_t shader_data = shader_data_t();
			shader_data.position = glm::vec3(1.2f, 1.0f, 2.0f);
			shader_data.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
			shader_data.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			shader_data.specular = glm::vec3(1.0f, 1.0f, 1.0f);
			shader_data.constant = 1.f;
			shader_data.linear = 0.09f;
			shader_data.quadratic = 0.032f;

			GLuint ubo = 0;
			glGenBuffers(1, &ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			unsigned int block_index = glGetUniformBlockIndex(program, "shader_data");

			GLuint binding_point_index = 2;
			glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, ubo);
			glUniformBlockBinding(program, block_index, binding_point_index);


			GLint lightAmbientLoc = glGetUniformLocation(program, "lights[0].ambient");
			GLint lightDiffuseLoc = glGetUniformLocation(program, "lights[0].diffuse");
			GLint lightSpecularLoc = glGetUniformLocation(program, "lights[0].specular");
			GLint lightPositionLoc = glGetUniformLocation(program, "lights[0].position");

			glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
			glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f); 
			glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
			glUniform3f(lightPositionLoc, lightPos.x, lightPos.y, lightPos.z);

			glUniform1f(glGetUniformLocation(program, "lights[0].constant"), 1.0f);
			glUniform1f(glGetUniformLocation(program, "lights[0].linear"), 0.09);
			glUniform1f(glGetUniformLocation(program, "lights[0].quadratic"), 0.032);


			laytex.draw();

			glm::mat4 view;
			glm::mat4 projection;
			GLint modelLoc = glGetUniformLocation(program, "model");

			camera->bind(*shader);
			

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


			camera->bind(*lamp);
			// Also draw the lamp object, again binding the appropriate shader
			lamp->use();
			// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
			modelLoc = glGetUniformLocation(lamp->program, "model");

			model = glm::mat4();
			model = glm::translate(model, lightPos);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			// Draw the light object (using light's vertex attributes)
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}

		virtual void handleInput(const InputManager& input) {


		}

		virtual void quit() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}
	};

}

#include "../renderer/lighting/lightmanager.h"
#include "../renderer/cameras/perspectivecamera.h"


void a_lighting() {
	

	RenderWindow* window = new RenderWindow("CameraTest", 1500, 850, true);
	InputManager* manager = new InputManager();

	LightManager* lightManager = new LightManager();
	ShaderManager* shaderManager = new ShaderManager(lightManager);

	manager->defineButton("Forward", GLFW_KEY_W);
	manager->defineButton("Forward", GLFW_KEY_A);

	geeL::Transform* transform = new geeL::Transform(glm::vec3(0.0f, 0.0f, 3.0f), vec3(0.f, -90.f, 0.f), vec3(1.f, 1.f, 1.f));
	geeL::Transform* transform2 = new geeL::Transform(glm::vec3(0.0f, 0.0f, 3.0f), vec3(0.f, -90.f, 0.f), vec3(1.f, 1.f, 1.f));
	geeL::Transform* transform3 = new geeL::Transform(glm::vec3(0.0f, 0.0f, 3.0f), vec3(0.f, -90.f, 0.f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera* camera = new PerspectiveCamera(transform, 5.f, 0.5f, 45.f, window->width, window->height, 0.1f, 100.f);
	PerspectiveCamera* camera2 = new PerspectiveCamera(transform2, 5.f, 0.5f, 65.f, window->width, window->height, 0.1f, 100.f);
	PerspectiveCamera* camera3 = new PerspectiveCamera(transform3, 5.f, 0.5f, 65.f, window->width, window->height, 0.1f, 100.f);

	SplitRenderer renderer = SplitRenderer(window, manager, shaderManager);
	SimpleRenderer* renderer1 = new SimpleRenderer(window, manager, shaderManager);
	SimpleRenderer* renderer2 = new SimpleRenderer(window, manager, shaderManager);
	SimpleRenderer* renderer3 = new SimpleRenderer(window, manager, shaderManager);
	
	renderer1->setCamera(camera);
	renderer2->setCamera(camera2);
	renderer3->setCamera(camera3);

	RenderViewport view1 = RenderViewport(0.f, 0.f, 0.5f, 0.5f);
	RenderViewport view2 = RenderViewport(0.5f, 0.5f, 0.5f, 0.5f);
	RenderViewport view3 = RenderViewport(0.f, 0.5f, 0.5f, 0.5f);

	renderer.addRenderer(renderer1, view2);
	renderer.addRenderer(renderer2, view1);
	renderer.addRenderer(renderer3, view3);
	
	
	shaderManager->addShader("Standard", new Shader("renderer/shaders/lighting.vert", "renderer/shaders/lighting.frag"));
	shaderManager->addShader("Lamp", new Shader("renderer/shaders/lighting.vert", "renderer/shaders/lamp.frag"));

	LightTestObject* testObj = new LightTestObject(window, shaderManager);
	renderer1->addObject(testObj);
	renderer2->addObject(testObj);
	renderer3->addObject(testObj);
	
	renderer.init();
	renderer.render();

	
	delete renderer1;
	delete renderer2;
	delete renderer3;
	delete testObj;
	delete window;
	delete manager;
	delete shaderManager;
	delete lightManager;
	delete camera;
	delete camera2;
	delete camera3;
}