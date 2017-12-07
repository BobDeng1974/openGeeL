#ifndef SIMPLEPOOL_H
#define SIMPLEPOOL_H

#include "memory.h"

namespace geeL {
	namespace memory {

		struct Chunk {
			Chunk* previous;
			Chunk* next;
			WORD dataSize;
			bool used;

			Chunk(WORD dataSize);

			void write(void* destination);
		};


		class SimplePool : public MemoryPool {

		public:
			SimplePool(WORD totalSize);
			~SimplePool();

			virtual void* allocate(WORD size);
			virtual void  deallocate(void* data);

		private:
			static const BYTE minChunkSize = 16;
			BYTE* poolMemory;

			Chunk* currentUsed;
			Chunk* currentFree;

			bool inBounds(void* data) const;

		};
	}
}

#endif