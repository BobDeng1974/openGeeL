#include "../texturing/rendertexturecube.h"
#include "cubebuffer.h"

namespace geeL {

	CubeBuffer::CubeBuffer() {
		glGenFramebuffers(1, &info.fbo);
		glGenRenderbuffers(1, &rbo);
	}

	CubeBuffer::CubeBuffer(const CubeBuffer& buffer) : FrameBuffer(buffer), rbo(buffer.rbo) {}

	CubeBuffer::~CubeBuffer() {
		remove();
	}

	void CubeBuffer::init(unsigned int resolution, unsigned int textureID) {
		this->textureID = textureID;
		info.width = resolution;
		info.height = resolution;

		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void CubeBuffer::init(const TextureCube& texture) {
		init(texture.getResolution(), texture.getID());
	}


	void CubeBuffer::fill(std::function<void()> drawCall) {
		glViewport(0, 0, info.width, info.height);

		FrameBuffer::bind(info.fbo);
		for (unsigned int side = 0; side < 6; side++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, textureID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawCall();
		}

		FrameBuffer::unbind();
	}

	void CubeBuffer::fill(std::function<void(unsigned int)> drawCall, unsigned int mipLevel) {
		glViewport(0, 0, info.width, info.height);

		FrameBuffer::bind(info.fbo);
		for (unsigned int side = 0; side < 6; side++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, textureID, mipLevel);
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
			"\n -- Texture " + std::to_string(textureID) + "\n";
	}

}