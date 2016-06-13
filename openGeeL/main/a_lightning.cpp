#include "a_lightning.h"

#include <iostream>
#include "../renderer/cameras/camera.h"
#include "../renderer/cameras/perspectivecamera.h"
#include "../renderer/lighting/light.h"
#include "../renderer/lighting/lightmanager.h"
#include "../renderer/lighting/pointlight.h"

#include "../renderer/transformation/transform.h"
#include "../renderer/transformation/transformfactory.h"

#include "../renderer/materials/material.h"
#include "../renderer/materials/materialfactory.h"
#include "../renderer/shader/shadermanager.h"
#include "../renderer/meshes/mesh.h"
#include "../renderer/meshes/model.h"
#include "../renderer/meshes/meshrenderer.h"
#include "../renderer/meshes/meshfactory.h"


#define pi 3.141592f

namespace {

	class LightTestObject : public RenderObject {

	public:

		Shader* shader;
		const Shader* lamp;
		GLuint VBO, VAO, containerVAO, lightVAO;
		GLuint texture1, texture2;
		SimpleTexture* texmex;
		SimpleTexture* spectex;
		LightManager manager;
		MaterialFactory factory;
		ShaderManager shaderManager;

		Model* nano;
		MeshRenderer* nanoRenderer;
		MeshFactory meshFactory;

		Material* material;
		
		glm::vec3 cubePositions[10];

		LightTestObject() : shaderManager(factory) {}

		virtual void init(const Camera* const camera) {

			//shader = &factory.defaultShader;
			shader = &factory.CreateShader("renderer/shaders/lighting.vert", "renderer/shaders/lighting.frag");
			lamp = &factory.CreateShader("renderer/shaders/lighting.vert", "renderer/shaders/lamp.frag");
			material = &factory.CreateMaterial(*shader);

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

			glGenVertexArrays(1, &containerVAO);
			glGenBuffers(1, &VBO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindVertexArray(containerVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
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

			PointLight* light1 = new PointLight(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.2f, 0.2f, 0.2f));
			PointLight* light2 = new PointLight(glm::vec3(-2.0f, 2.0f, -7.0f), glm::vec3(0.3f, 0.3f, 0.9f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f, 0.1f, 0.1f));

			manager = LightManager();
			manager.addLight(light1);
			manager.addLight(light2);

			texmex = &factory.CreateTexture("data/images/container2.png");
			spectex = &factory.CreateTexture("data/images/container2_specular.png");
			
			shaderManager.staticBind(manager);

			material->addTexture("material.diffuse", *texmex);
			material->addTexture("material.specular", *spectex);
			material->addParameter("material.shininess", 64.f);
			material->useCamera("camera");
			material->useLights("", "", "");
			material->staticBind(manager);

			nano = &meshFactory.CreateModel("resources/nanosuit/nanosuit.obj", factory);

			geeL::Transform* transi = new geeL::Transform(glm::vec3(0.0f, -5.75f, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.2f, 0.2f, 0.2f));

			nanoRenderer = new MeshRenderer(*transi, *nano);
		}

		//glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
		virtual void draw(const Camera* const camera) {
			
			GLuint program = shader->program;

			shaderManager.dynamicBind(manager, *camera);

			material->dynamicBind(manager, *camera);
			GLint modelLoc = glGetUniformLocation(program, "model");

			glm::mat4 model;
			glBindVertexArray(containerVAO);
			for (GLuint i = 0; i < 10; i++) {

				model = glm::mat4();
				model = glm::translate(model, cubePositions[i]);
				GLfloat angle = 20.0f * i;
				model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			glBindVertexArray(0);
			

			nanoRenderer->draw(manager, *camera);

			/*
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
			*/
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

	manager->defineButton("Forward", GLFW_KEY_W);
	manager->defineButton("Forward", GLFW_KEY_A);


	geeL::Transform world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory transFactory = TransformFactory(world);

	geeL::Transform& transform = transFactory.CreateTransform(glm::vec3(0.0f, 0.0f, 3.0f), vec3(0.f, -90.f, 0.f), vec3(1.f, 1.f, 1.f));
	geeL::Transform& transform2 = transFactory.CreateTransform(glm::vec3(0.0f, 0.0f, 3.0f), vec3(0.f, -90.f, 0.f), vec3(1.f, 1.f, 1.f));
	geeL::Transform& transform3 = transFactory.CreateTransform(glm::vec3(0.0f, 0.0f, 3.0f), vec3(0.f, -90.f, 0.f), vec3(1.f, 1.f, 1.f));
	
	PerspectiveCamera camera = PerspectiveCamera(transform, 5.f, 15.f, 45.f, window->width, window->height, 0.1f, 100.f);
	PerspectiveCamera camera2 = PerspectiveCamera(transform2, 5.f, 15.f, 65.f, window->width, window->height, 0.1f, 100.f);
	PerspectiveCamera camera3 = PerspectiveCamera(transform3, 5.f, 15.f, 65.f, window->width, window->height, 0.1f, 100.f);

	SplitRenderer renderer = SplitRenderer(window, manager);
	SimpleRenderer renderer1 = SimpleRenderer(window, manager);
	SimpleRenderer renderer2 = SimpleRenderer(window, manager);
	SimpleRenderer renderer3 = SimpleRenderer(window, manager);
	
	renderer1.setCamera(&camera);
	renderer2.setCamera(&camera2);
	renderer3.setCamera(&camera3);

	RenderViewport view1 = RenderViewport(0.f, 0.f, 0.5f, 0.5f);
	RenderViewport view2 = RenderViewport(0.5f, 0.5f, 0.5f, 0.5f);
	RenderViewport view3 = RenderViewport(0.f, 0.5f, 0.5f, 0.5f);

	renderer.addRenderer(&renderer1, view2);
	renderer.addRenderer(&renderer2, view1);
	renderer.addRenderer(&renderer3, view3);
	
	
	LightTestObject* testObj = new LightTestObject();
	renderer1.addObject(testObj);
	renderer2.addObject(testObj);
	renderer3.addObject(testObj);
	
	renderer.init();
	renderer.render();
	
	delete testObj;
	delete window;
	delete manager;
	delete lightManager;
}