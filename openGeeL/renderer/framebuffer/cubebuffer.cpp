#define GLEW_STATIC
#include <glew.h>
#include "texturing/rendertexturecube.h"
#include "utility/viewport.h"
#include "cubebuffer.h"


namespace geeL {

	CubeBuffer::CubeBuffer() : texture(nullptr) {
		glGenFramebuffers(1, &fbo.token);
		glGenRenderbuffers(1, &rbo);
	}


	void CubeBuffer::init(const TextureCube& texture) {
		this->texture = &texture;
		this->resolution = Resolution(texture.getResolution());

		bind();
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, texture.getResolution(), texture.getResolution());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		unbind();
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}


	void CubeBuffer::fill(std::function<void()> drawCall) {

		bind();
		for (unsigned int side = 0; side < 6; side++) {
			Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, texture->getID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawCall();
		}

		unbind();
	}

	void CubeBuffer::fill(std::function<void(unsigned int)> drawCall, unsigned int mipLevel) {

		bind();
		for (unsigned int side = 0; side < 6; side++) {
			Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, texture->getID(), mipLevel);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawCall(side);
		}

		unbind();
	}


	void CubeBuffer::resize(ResolutionScale scale) {
		resolution.resize(scale);

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution.getWidth(), resolution.getHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	std::string CubeBuffer::toString() const {
		return "Cube buffer " + std::to_string(fbo.token) +
			"\n -- Texture " + std::to_string(texture->getID()) + "\n";
	}

}