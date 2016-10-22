#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>

#include "../utility/rendertime.h"
#include "../shader/shader.h"
#include "../postprocessing/postprocessing.h"
#include "../postprocessing/drawdefault.h"
#include "postrenderer.h"
#include "../window.h"
#include "../scripting/scenecontrolobject.h"
#include "../inputmanager.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"
#include "../shader/shadermanager.h"
#include "../scene.h"

#define fps 10

namespace geeL {


	PostProcessingRenderer::PostProcessingRenderer(RenderWindow* window, InputManager* inputManager)
		: Renderer(window, inputManager), effect(nullptr) {

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			std::cout << "Failed to initialize GLEW" << std::endl;
		}

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
	}

	
	void exitCallbackk(GLFWwindow* window, int key, int scancode, int action, int mode) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	void PostProcessingRenderer::init() {
		inputManager->addCallback(exitCallbackk);
		inputManager->init(window);
		
		initFrameBuffer();
		initScreenQuad();
	}

	// Generates a texture for framebuffer
	GLuint generateTexture(bool color, int screenWidth, int screenHeight) {

		//Generate texture ID and load texture data 
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		if (color) {
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		}
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}

	void PostProcessingRenderer::initFrameBuffer() {

		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		// Create a color attachment texture
		colorBuffer = generateTexture(true, window->width, window->height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

		// Create a renderbuffer object for depth and stencil attachment
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window->width, window->height); 
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 
																									  
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//Create quad that will display rendered image on screen
	void PostProcessingRenderer::initScreenQuad() {

		GLfloat quadVertices[] = {   
			//Positions    //Tex Coords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};

		glGenVertexArrays(1, &screenVAO);
		glGenBuffers(1, &screenVBO);
		glBindVertexArray(screenVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
		glBindVertexArray(0);
	}


	void PostProcessingRenderer::render() {

		DefaultPostProcess defaultEffect = DefaultPostProcess();
		shaderManager->staticBind(*scene);

		while (!window->shouldClose()) {
			int currFPS = ceil(Time::deltaTime * 1000.f);
			std::this_thread::sleep_for(std::chrono::milliseconds(fps - currFPS));
			glfwPollEvents();
			inputManager->update();
			handleInput();

			glEnable(GL_DEPTH_TEST);
			scene->lightManager.drawShadowmaps(*scene);

			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glViewport(0, 0, window->width, window->height);
			glClearColor(0.002f, 0.002f, 0.002f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderFrame();
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			
			if (effect != nullptr) {
				effect->draw();
				effect->bindToScreen(screenVAO, colorBuffer);
			}
			//Default rendering
			else {
				defaultEffect.draw();
				defaultEffect.bindToScreen(screenVAO, colorBuffer);
			}

			window->swapBuffer();
			Time::update();
		}

		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->quit();

		window->close();
	}

	void PostProcessingRenderer::renderFrame() {
		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->draw(scene->camera);

		shaderManager->dynamicBind(*scene);
		scene->draw();
	}

	void PostProcessingRenderer::handleInput() {
		scene->camera.handleInput(*inputManager);
	}

	void PostProcessingRenderer::setEffect(PostProcessingEffect& effect) {
		this->effect = &effect;
	}

}