#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "../renderer.h"
#include "framebuffer.h"

using namespace std;

namespace geeL {

	FrameBuffer::FrameBuffer(int width, int height) 
		: width(width), height(height) {}


	void FrameBuffer::init() {
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// Create a color attachment texture
		color = generateTexture(true);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

		// Create a renderbuffer object for depth and stencil attachment
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::fill(Drawer& drawer) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, width, height);
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawer.draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int FrameBuffer::generateTexture(bool color) {

		//Generate texture ID and load texture data 
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		if (color) {
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		}
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}
}