#ifndef BUFFERUTILITY_H
#define BUFFERUTILITY_H

namespace geeL {

	struct TextureBuffer {

	public:
		unsigned int texture;
		unsigned int buffer;

		~TextureBuffer();
	};


	class BufferUtility {

	public:
		static void generateAtomicBuffer(unsigned int& buffer);
		static unsigned int* getAtomicBufferCount(unsigned int& buffer);
		static void resetAtomicBuffer(unsigned int& buffer);
		static void resetAtomicBuffer(unsigned int& buffer, unsigned int* count);
	
		static void generateTextureBuffer(int size, unsigned int format, 
			unsigned int& texture, unsigned int& buffer);

		static void generateTextureBuffer(int size, unsigned int format,
			TextureBuffer&buffer);


	};
}

#endif