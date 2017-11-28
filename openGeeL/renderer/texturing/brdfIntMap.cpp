#define GLEW_STATIC
#include <glew.h>
#include "stb_image.h"
#include "primitives/screenquad.h"
#include "shader/rendershader.h"
#include "framebuffer/framebuffer.h"
#include "glwrapper/viewport.h"
#include "brdfIntMap.h"

namespace geeL {

	BRDFIntegrationMap::BRDFIntegrationMap() : Texture2D(ColorType::None, Resolution(512)) {
		RenderShader* shader = new RenderShader("shaders/screen.vert", "shaders/cubemapping/brdfIntMap.frag");

		unsigned int resolution = 512;
		unsigned int fbo, rbo;

		glGenFramebuffers(1, &fbo);
		glGenRenderbuffers(1, &rbo);

		FrameBuffer::bind(fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, resolution, resolution, 0, GL_RG, GL_FLOAT, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Reconfigure capture framebuffer object and render quad with BRDF shader.
		FrameBuffer::bind(fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);

		shader->bindParameters();
		Viewport::set(0, 0, resolution, resolution);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ScreenQuad::get().drawComplete();

		FrameBuffer::unbind();
		glDeleteFramebuffers(1, &fbo);

		delete shader;
	}

}