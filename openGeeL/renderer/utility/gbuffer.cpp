#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "../shader/shader.h"
#include "../renderer.h"
#include "gbuffer.h"

namespace geeL {

	GBuffer::GBuffer() {}


	void GBuffer::init(int width, int height) {
		this->width = width;
		this->height = height;

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//Create attachements for all color buffers
		positionDepth = generateTexture(0, GL_RGBA16F);
		normalMet     = generateTexture(1, GL_RGBA16F);
		diffuseSpec   = generateTexture(2, GL_RGBA);
		GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		// Create a renderbuffer object for depth and stencil attachment
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << "\n";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GBuffer::fill(Drawer& drawer) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, width, height);
		glClearColor(0.0001f, 0.0001f, 0.0001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawer.draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int GBuffer::generateTexture(unsigned int position, unsigned int type) {
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, 
			(type == GL_RGB16F) ? GL_RGB : GL_RGBA, (type == GL_RGB16F || type == GL_RGBA16F) ? GL_FLOAT : GL_UNSIGNED_BYTE, NULL);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + position, GL_TEXTURE_2D, textureID, 0);

		return textureID;
	}

}