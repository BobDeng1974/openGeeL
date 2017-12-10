#ifndef DEFAULTPOOL_H
#define DEFAULTPOOL_H

#include "memory.h"

namespace geeL {
	namespace memory {

		class DefaultMemory : public Memory {

		public:
			virtual ~DefaultMemory() {}

			virtual void* allocate(size_t size);
			virtual void  deallocate(void* data);

		};


		inline void* DefaultMemory::allocate(size_t size) {
			return ::operator new(size);
		}

		inline void DefaultMemory::deallocate(void* data) {
			::operator delete(data);
		}

	}
}

#endif
