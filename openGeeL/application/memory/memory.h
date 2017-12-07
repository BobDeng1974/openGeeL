#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

namespace geeL { 
	namespace memory {

		typedef unsigned int WORD;
		typedef unsigned char BYTE;


		class Memory {

			public:
				virtual void* allocate(WORD size) = 0;
				virtual void  deallocate(void* data) = 0;
		};


		class MemoryPool : public Memory {

		public:
			virtual void* allocate(WORD size) = 0;
			virtual void  deallocate(void* data) = 0;

			WORD getFreeMemorySize() const;
			WORD getTotalMemorySize() const;

		protected:
			WORD freeMemory;
			WORD totalMemory;

			MemoryPool();
			virtual ~MemoryPool() {}

		};


		inline MemoryPool::MemoryPool()
			: freeMemory(0)
			, totalMemory(0) {}

		inline WORD MemoryPool::getFreeMemorySize() const {
			return freeMemory;
		}

		inline WORD MemoryPool::getTotalMemorySize() const {
			return totalMemory;
		}

	} 
}

#endif
