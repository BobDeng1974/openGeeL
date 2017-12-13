#ifndef MEMORYOBJECT_H
#define MEMORYOBJECT_H

#include <functional>
#include <memory>

namespace geeL {

	template<typename T>
	using del_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;


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
