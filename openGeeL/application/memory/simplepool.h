#ifndef SIMPLEPOOL_H
#define SIMPLEPOOL_H

#include "memory.h"

namespace geeL {
	namespace memory {

		struct Chunk {
			Chunk* previous;
			Chunk* next;
			size_t dataSize;
			bool used;

			Chunk(size_t dataSize);

			void write(void* destination);
		};


		class SimplePool : public MemoryPool {

		public:
			SimplePool(size_t totalSize);
			virtual ~SimplePool();

			virtual void* allocate(size_t size);
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
