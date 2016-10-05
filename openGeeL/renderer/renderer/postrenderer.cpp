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
#include "postrenderer.h"
#include "../window.h"
#include "../renderobject.h"
#include "../inputmanager.h"
#include "../cameras/camera.h"

#define fps 10

namespace geeL {


	PostProcessingRenderer::PostProcessingRenderer(RenderWindow* window, InputManager* inputManager)
		: Renderer(window, inputManager), effect(nullptr) {

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			std::cout << "Failed to initialize GLEW" << std::endl;
		}

		glViewport(0, 0, window->width, window->height);
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
		for (size_t i = 0; i < objects.size(); i++) {
			objects[i]->init(currentCamera);
		}

		inputManager->addCallback(exitCallbackk);
		inputManager->init(window);
		
		initFrameBuffer();
		initScreenQuad();
	}

	// Generates a texture that is suited for attachments to a framebuffer
	GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil, int screenWidth, int screenHeight) {

		// What enum to use?
		GLenum attachment_type;
		if (!depth && !stencil)
			attachment_type = GL_RGB;
		else if (depth && !stencil)
			attachment_type = GL_DEPTH_COMPONENT;
		else if (!depth && stencil)
			attachment_type = GL_STENCIL_INDEX;

		//Generate texture ID and load texture data 
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		if (!depth && !stencil)
			glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, screenWidth, screenHeight, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
		else // Using both a stencil and depth test, needs special format arguments
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, screenWidth, screenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}

	void PostProcessingRenderer::initFrameBuffer() {

		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		// Create a color attachment texture
		colorBuffer = generateAttachmentTexture(false, false, window->width, window->height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

		// Create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window->width, window->height); 
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 
																									  
		// Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PostProcessingRenderer::initScreenQuad() {

		// Vertex attributes for a quad that fills the entire screen
		GLfloat quadVertices[] = {   
			// Positions   // TexCoords
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

		while (!window->shouldClose()) {
			int currFPS = ceil(Time::deltaTime * 1000.f);
			std::this_thread::sleep_for(std::chrono::milliseconds(fps - currFPS));

			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			renderFrame();
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			if (effect != nullptr) {
				effect->draw();
				effect->bindToScreen(screenVAO, colorBuffer);
			}

			window->swapBuffer();

			glfwPollEvents();
			inputManager->update();
			handleInput();

			Time::update();
		}

		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->quit();

		window->close();
	}

	void PostProcessingRenderer::renderFrame() {
		currentCamera->update();

		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->draw(currentCamera);

		currentCamera->drawSkybox();
	}

	void PostProcessingRenderer::handleInput() {
		currentCamera->handleInput(*inputManager);
	}

	void PostProcessingRenderer::setEffect(PostProcessingEffect& effect) {
		this->effect = &effect;
	}

	void PostProcessingRenderer::setCamera(Camera* camera) {
		if (camera != nullptr)
			currentCamera = camera;
	}


}