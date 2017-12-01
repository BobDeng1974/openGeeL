#define GLEW_STATIC
#include <glew.h>
#include "texturing/rendertexturecube.h"
#include "glwrapper/viewport.h"
#include "cubebuffer.h"


namespace geeL {

	CubeBuffer::CubeBuffer() 
		: texture(nullptr) {

		glGenFramebuffers(1, &fbo.token);
	}


	void CubeBuffer::init(const TextureCube& texture) {
		this->texture = &texture;
		this->resolution = texture.getScreenResolution();

		bind();
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution.getWidth(), resolution.getHeight());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		unbind();
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}


	void CubeBuffer::fill(std::function<void()> drawCall, Clearer clearer) {

		bind();
		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());
		for (unsigned int side = 0; side < 6; side++) {
			texture->assignSide(0, 0, side);
			clearer.clear();

			drawCall();
		}

		unbind();
	}

	void CubeBuffer::fill(std::function<void(unsigned int)> drawCall, unsigned int mipLevel, Clearer clearer) {

		bind();
		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());
		for (unsigned int side = 0; side < 6; side++) {
			texture->assignSide(0, mipLevel, side);
			clearer.clear();

			drawCall(side);
		}

		unbind();
	}


	Resolution CubeBuffer::getResolution() const {
		assert(texture != nullptr);
		return texture->getScreenResolution();
	}

	void CubeBuffer::resize(ResolutionScale scale) {
		resolution.resize(scale);

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution.getWidth(), resolution.getHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	std::string CubeBuffer::toString() const {
		return "Cube buffer " + std::to_string(fbo.token) + "\n";
	}

}