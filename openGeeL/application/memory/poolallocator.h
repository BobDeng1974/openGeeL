#ifndef POOLALLOCATOR_H
#define POOLALLOCATOR_H

#include "appmanager.h"
#include "memory.h"

namespace geeL {
	namespace memory {


		//Allocator that uses given memory class for allocation/deallocation
		template<typename T>
		class PoolAllocator {
		public:
			typedef T value_type;
			typedef value_type* pointer;
			typedef const value_type* const_pointer;
			typedef value_type& reference;
			typedef const value_type& const_reference;
			typedef std::size_t size_type;
			typedef std::ptrdiff_t difference_type;

		public:
			template<typename U>
			struct rebind {
				typedef PoolAllocator<U> other;
			};

		public:
			inline explicit PoolAllocator() : memory(nullptr) {}
			inline explicit PoolAllocator(Memory& memory) : memory(&memory) {}
			inline ~PoolAllocator() {}
			inline explicit PoolAllocator(PoolAllocator const& other) 
				: memory(other.memory) {}

			template<typename U>
			inline explicit PoolAllocator(PoolAllocator<U> const& other) 
				: memory(other.memory) {}

			inline pointer address(reference r) { return &r; }
			inline const_pointer address(const_reference r) { return &r; }

			inline pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0) {
				void* data = memory->allocate(cnt * sizeof(T));

				return reinterpret_cast<pointer>(data);
			}

			inline void deallocate(pointer p, size_type) {
				memory->deallocate(p);
			}

			inline size_type max_size() const {
				return std::numeric_limits<size_type>::max() / sizeof(T);
			}

			inline void construct(pointer p, const T& t) { new(p) T(t); }
			inline void destroy(pointer p) { p->~T(); }

			inline bool operator==(PoolAllocator const&) { return true; }
			inline bool operator!=(PoolAllocator const& a) { return !operator==(a); }

		private:
			Memory* memory;

		};


		//Allocator that uses memory of currently running application
		template<typename T>
		class CurrentPoolAllocator {
		public:
			typedef T value_type;
			typedef value_type* pointer;
			typedef const value_type* const_pointer;
			typedef value_type& reference;
			typedef const value_type& const_reference;
			typedef std::size_t size_type;
			typedef std::ptrdiff_t difference_type;

		public:
			template<typename U>
			struct rebind {
				typedef CurrentPoolAllocator<U> other;
			};

		public:
			inline explicit CurrentPoolAllocator() {}
			inline ~CurrentPoolAllocator() {}
			inline explicit CurrentPoolAllocator(CurrentPoolAllocator const&) {}

			template<typename U>
			inline explicit CurrentPoolAllocator(CurrentPoolAllocator<U> const&) {}

			inline pointer address(reference r) { return &r; }
			inline const_pointer address(const_reference r) { return &r; }

			inline pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0) {
				Memory& memory = ApplicationManager::getCurrentMemory();
				void* data = memory.allocate(cnt * sizeof(T));

				return reinterpret_cast<pointer>(data);
			}

			inline void deallocate(pointer p, size_type) {
				Memory& memory = ApplicationManager::getCurrentMemory();
				memory.deallocate(p);
			}

			inline size_type max_size() const {
				return std::numeric_limits<size_type>::max() / sizeof(T);
			}

			inline void construct(pointer p, const T& t) { new(p) T(t); }
			inline void destroy(pointer p) { p->~T(); }

			inline bool operator==(CurrentPoolAllocator const&) { return true; }
			inline bool operator!=(CurrentPoolAllocator const& a) { return !operator==(a); }
		};

	}
}

#endif
