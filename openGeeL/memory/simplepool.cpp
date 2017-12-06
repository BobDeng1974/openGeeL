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


		SimplePool::SimplePool(WORD totalSize) {
			totalMemory = totalSize;
			freeMemory  = totalSize - sizeof(Chunk);

			poolMemory = ::new BYTE[totalSize];

			Chunk firstChunk(freeMemory);
			firstChunk.write(poolMemory);

			freeChunks.emplace(poolMemory);
		}

		SimplePool::~SimplePool(){
			::delete[] poolMemory;
		}


		void* SimplePool::allocate(WORD size) {
			WORD newSize = size + sizeof(Chunk);

			Chunk* chunk = nullptr;
			for (auto it(freeChunks.begin()); it != freeChunks.end(); it++) {
				void* dat = *it;

				Chunk* currChunk = reinterpret_cast<Chunk*>(dat);
				if (!currChunk->used && size <= currChunk->dataSize) {
					chunk = currChunk;
					break;
				}
			}

			assert(chunk != nullptr);

			freeMemory -= newSize;
			chunk->used = true;
			BYTE* data = reinterpret_cast<BYTE*>(chunk);

			//Split chunks if new size is sufficient enough 
			WORD newFreeSize = chunk->dataSize - size;
			if (newFreeSize >= minChunkSize) {
				Chunk newChunk(newFreeSize);

				newChunk.previous = chunk;
				newChunk.next = chunk->next;

				newChunk.write(data + newSize); //Move it behind current chunk
				Chunk* allocatedChunk = reinterpret_cast<Chunk*>(data + newSize);

				if (allocatedChunk->next != nullptr) {
					Chunk* nextChunk = allocatedChunk->next;
					nextChunk->previous = allocatedChunk;
				}

				chunk->next = allocatedChunk;
				chunk->dataSize = size;

				freeChunks.emplace(allocatedChunk);
				freeChunks.erase(chunk);
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

			Chunk* headChunk = chunk;
			Chunk* previous = chunk->previous;
			Chunk* next		= chunk->next;

			//Try merging chunks if neighours are also unused

			//Previous chunk is unused and we need to merge
			if (previous != nullptr && !previous->used) {
				freeChunks.erase(previous);
				headChunk = previous;
				previous  = headChunk->previous;
				next	  = chunk->next;

				chunkSize += headChunk->dataSize + sizeof(Chunk);

				if (next != nullptr)
					next->previous = headChunk;
			}

			//Next chunk is unused and we need to merge
			if (next != nullptr && !next->used) {
				freeChunks.erase(next);
				chunkSize += next->dataSize + sizeof(Chunk);
				next = next->next;

				if (next != nullptr)
					next->previous = headChunk;
			}


			BYTE* newData = reinterpret_cast<BYTE*>(headChunk);
			WORD newDataSize = chunkSize - sizeof(Chunk);
			Chunk newChunk(newDataSize);
			newChunk.previous = previous;
			newChunk.next = next;

			newChunk.write(newData);
			freeChunks.emplace(newData);
		}

	}
}