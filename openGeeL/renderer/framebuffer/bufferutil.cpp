#define GLEW_STATIC
#include <glew.h>
#include "bufferutil.h"

namespace geeL {

	void BufferUtility::generateAtomicBuffer(unsigned int& buffer) {
		if (buffer)
			glDeleteBuffers(1, &buffer);

		unsigned int index = 0;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), &index, GL_STATIC_DRAW);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}

	void BufferUtility::generateTextureBuffer(int size, unsigned int format, unsigned int& texture, unsigned int& buffer) {
		//Generate texture buffer
		if (buffer)
			glDeleteBuffers(1, &buffer);

		glBindBuffer(GL_TEXTURE_BUFFER, buffer);
		glBufferData(GL_TEXTURE_BUFFER, size, 0, GL_STATIC_DRAW);

		//Generate containing texture
		if (texture > 0)
			glDeleteTextures(1, &texture);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_BUFFER, texture);
		glTexBuffer(GL_TEXTURE_BUFFER, format, buffer);

		glBindBuffer(GL_TEXTURE_BUFFER, 0);
		//glBindTexture(GL_TEXTURE_BUFFER, 0);
	}

	void BufferUtility::generateTextureBuffer(int size, unsigned int format, TextureBuffer&buffer) {
		generateTextureBuffer(size, format, buffer.texture, buffer.buffer);
	}

}