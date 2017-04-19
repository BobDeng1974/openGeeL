#include "cubebuffer.h"

namespace geeL {


	void CubeBuffer::init(unsigned int resolution, unsigned int textureID) {
		this->textureID = textureID;
		info.width = resolution;
		info.height = resolution;

		unsigned int rbo;
		glGenFramebuffers(1, &info.fbo);
		glGenRenderbuffers(1, &rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, info.fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}


	void CubeBuffer::fill(std::function<void()> drawCall) {
		glViewport(0, 0, info.width, info.height);

		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		FrameBuffer::bind(info.fbo);
		for (unsigned int side = 0; side < 6; side++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, textureID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawCall();
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		FrameBuffer::unbind();
	}

	void CubeBuffer::fill(std::function<void(unsigned int)> drawCall) {
		glViewport(0, 0, info.width, info.height);

		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		FrameBuffer::bind(info.fbo);
		for (unsigned int side = 0; side < 6; side++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, textureID, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawCall(side);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		FrameBuffer::unbind();
	}

}