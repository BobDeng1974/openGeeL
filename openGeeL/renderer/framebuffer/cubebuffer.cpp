#include "../texturing/rendertexturecube.h"
#include "cubebuffer.h"

namespace geeL {

	CubeBuffer::CubeBuffer() : texture(nullptr) {
		glGenFramebuffers(1, &info.fbo);
		glGenRenderbuffers(1, &rbo);
	}

	CubeBuffer::~CubeBuffer() {
		remove();
	}

	void CubeBuffer::init(const TextureCube& texture) {
		this->texture = &texture;

		info.width = texture.getResolution();
		info.height = texture.getResolution();

		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, texture.getResolution(), texture.getResolution());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}


	void CubeBuffer::fill(std::function<void()> drawCall) {
		glViewport(0, 0, info.width, info.height);

		FrameBuffer::bind(info.fbo);
		for (unsigned int side = 0; side < 6; side++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, texture->getID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawCall();
		}

		FrameBuffer::unbind();
	}

	void CubeBuffer::fill(std::function<void(unsigned int)> drawCall, unsigned int mipLevel) {
		glViewport(0, 0, info.width, info.height);

		FrameBuffer::bind(info.fbo);
		for (unsigned int side = 0; side < 6; side++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, texture->getID(), mipLevel);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawCall(side);
		}

		FrameBuffer::unbind();
	}

	void CubeBuffer::resize(unsigned int width, unsigned int height) {
		info.width = width;
		info.height = height;

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	std::string CubeBuffer::toString() const {
		return "Cube buffer " + std::to_string(info.fbo) + 
			"\n -- Texture " + std::to_string(texture->getID()) + "\n";
	}

}