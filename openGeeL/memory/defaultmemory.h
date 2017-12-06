#ifndef DEFAULTPOOL_H
#define DEFAULTPOOL_H

#include "memory.h"

namespace geeL {
	namespace memory {

		class DefaultMemory : public Memory {

		public:
			virtual void* allocate(WORD size);
			virtual void  deallocate(void* data);

		};


		inline void* DefaultMemory::allocate(WORD size) {
			return ::operator new(size);
		}

		inline void DefaultMemory::deallocate(void* data) {
			::operator delete(data);
		}

	}
}

#endif
