#ifndef MEMORYOBJECT_H
#define MEMORYOBJECT_H

#include <memory>

namespace geeL {
	namespace memory {

		template<typename T>
		class MemoryObject : public std::shared_ptr<T> {

		public:
			template<typename ...PointerArgs>
			MemoryObject(PointerArgs&& ...args);

		};


		template<typename T>
		template<typename ...PointerArgs>
		inline MemoryObject<T>::MemoryObject(PointerArgs&& ...args)
			: std::shared_ptr<T>(std::forward<PointerArgs>(args)...) {}

	}

}

#endif
