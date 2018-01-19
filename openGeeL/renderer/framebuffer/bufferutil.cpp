#define GLEW_STATIC
#include <glew.h>
#include <string>
#include "bufferutil.h"

namespace geeL {

	TextureBuffer::~TextureBuffer() {
		glDeleteTextures(1, &texture);
		glDeleteBuffers(1, &buffer);
	}

	void BufferUtility::generateAtomicBuffer(unsigned int& buffer) {
		if (buffer)
			glDeleteBuffers(1, &buffer);

		unsigned int index = 0;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), &index, GL_STATIC_DRAW);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}

	unsigned int * BufferUtility::getAtomicBufferCount(unsigned int& buffer) {
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
		return (unsigned int*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0,
			sizeof(unsigned int), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	}

	void BufferUtility::resetAtomicBuffer(unsigned int& buffer) {
		resetAtomicBuffer(buffer, getAtomicBufferCount(buffer));
	}

	void BufferUtility::resetAtomicBuffer(unsigned int& buffer, unsigned int* count) {
		memset(count, 0, sizeof(unsigned int));
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	}

	void BufferUtility::generateTextureBuffer(int size, unsigned int format, unsigned int& texture, unsigned int& buffer) {
		//Generate texture buffer
		if (buffer)
			glDeleteBuffers(1, &buffer);

		glGenBuffers(1, &buffer);
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

	void BufferUtility::drawBuffers(unsigned int size) {

		switch (size) {
			case 1: {
				glDrawBuffer(GL_COLOR_ATTACHMENT0);
			}
			case 2: {
				unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
				glDrawBuffers(2, attachments);
			}
			case 3: {
				unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
				glDrawBuffers(3, attachments);
			}
			case 4: {
				unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
					GL_COLOR_ATTACHMENT3 };
				glDrawBuffers(4, attachments);
			}
			case 5: {
				unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
					GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
				glDrawBuffers(5, attachments);
			}

			default: {
				unsigned int attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
					GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
				glDrawBuffers(6, attachments);
			}
		}

	}

}