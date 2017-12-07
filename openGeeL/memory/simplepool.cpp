#include <cassert>
#include <memory>
#include "simplepool.h"

namespace geeL {
	namespace memory {

		Chunk::Chunk(WORD dataSize)
			: previous(nullptr)
			, next(nullptr)
			, dataSize(dataSize)
			, used(false) {}

		void Chunk::write(void* destination) {
			memcpy(destination, this, sizeof(Chunk));
		}


		SimplePool::SimplePool(WORD totalSize) 
			: currentUsed(nullptr) {
			totalMemory = totalSize;
			freeMemory  = totalSize - sizeof(Chunk);

			poolMemory = ::new BYTE[totalSize];

			Chunk firstChunk(freeMemory);
			firstChunk.write(poolMemory);

			Chunk* allocatedChunk = reinterpret_cast<Chunk*>(poolMemory);
			currentFree = allocatedChunk;
		}

		SimplePool::~SimplePool(){
			::delete[] poolMemory;
		}


		void* SimplePool::allocate(WORD size) {
			WORD newSize = size + sizeof(Chunk);

			//Search fitting chunk via iteration
			Chunk* chunk = currentFree;
			while (chunk != nullptr) {
				if (size <= chunk->dataSize)
					break;

				chunk = chunk->next;
			}

			assert(chunk != nullptr);

			freeMemory -= newSize;
			chunk->used = true;
			BYTE* data = reinterpret_cast<BYTE*>(chunk);

			//Split chunks if new size is sufficient enough 
			WORD newFreeSize = chunk->dataSize - size;
			if (newFreeSize >= minChunkSize) {
				Chunk newChunk(newFreeSize);

				newChunk.next = chunk->next;
				newChunk.write(data + newSize); //Move it behind current chunk

				Chunk* allocatedChunk = reinterpret_cast<Chunk*>(data + newSize);
				currentFree = allocatedChunk;

				if (allocatedChunk->next != nullptr) {
					Chunk* nextChunk = allocatedChunk->next;
					nextChunk->previous = allocatedChunk;
				}

				chunk->previous = currentUsed;
				currentUsed = chunk;

				if (chunk->previous != nullptr) {
					Chunk* previousUsed = chunk->previous;
					previousUsed->next = chunk;
				}

				chunk->next = nullptr;
				chunk->dataSize = size;
			}

			return data + sizeof(Chunk);
		}

		void SimplePool::deallocate(void* data) {
			assert(data != nullptr);

			BYTE* d = reinterpret_cast<BYTE*>(data);
			Chunk* chunk = reinterpret_cast<Chunk*>(d - sizeof(Chunk));
			if (!chunk->used) return;

			WORD chunkSize = chunk->dataSize + sizeof(Chunk);
			freeMemory += chunkSize;

			Chunk* previousUsed = chunk->previous;
			Chunk* nextUsed		= chunk->next;

			if (previousUsed != nullptr)
				previousUsed->next = nextUsed;

			if (nextUsed != nullptr)
				nextUsed->previous = previousUsed;

			chunk->used = false;
			chunk->previous = nullptr;
			chunk->next = currentFree;
			currentFree->previous = chunk;
			currentFree = chunk;

			//Try merging chunks if next one (in memory) is also free
			
			void* nextData = d + chunk->dataSize;
			while (inBounds(nextData)) {
				Chunk* next = reinterpret_cast<Chunk*>(nextData);

				if (!next->used) {
					chunkSize += next->dataSize + sizeof(Chunk);

					//Remove this element from current linked list position
					Chunk* nextPrevious = next->previous;
					Chunk* nextNext     = next->next;

					if (nextPrevious != nullptr)
						nextPrevious->next = nextNext;

					if (nextNext != nullptr)
						nextNext->previous = nextPrevious;

					//Write new combined chunk into memory
					WORD newDataSize = chunkSize - sizeof(Chunk);
					chunk->dataSize = newDataSize;
				}
				else 
					break;

				BYTE* dat = reinterpret_cast<BYTE*>(next);
				nextData = dat + next->dataSize + sizeof(Chunk);
			}
		}

		bool SimplePool::inBounds(void* data) const {
			return data >= poolMemory && data < (poolMemory + totalMemory);
		}

	}
}