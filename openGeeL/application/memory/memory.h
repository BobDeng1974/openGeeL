#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

namespace geeL { 
	namespace memory {

		typedef unsigned char BYTE;

		class Memory {

			public:
				virtual ~Memory() {}

				virtual void* allocate(size_t size) = 0;
				virtual void  deallocate(void* data) = 0;
		};


		class MemoryPool : public Memory {

		public:
			virtual ~MemoryPool() {}

			virtual void* allocate(size_t size) = 0;
			virtual void  deallocate(void* data) = 0;

			size_t getFreeMemorySize() const;
			size_t getTotalMemorySize() const;

		protected:
			size_t freeMemory;
			size_t totalMemory;

			MemoryPool();
			

		};


		inline MemoryPool::MemoryPool()
			: freeMemory(0)
			, totalMemory(0) {}

		inline size_t MemoryPool::getFreeMemorySize() const {
			return freeMemory;
		}

		inline size_t MemoryPool::getTotalMemorySize() const {
			return totalMemory;
		}

	} 
}

#endif
