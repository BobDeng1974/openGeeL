#include "memory/memory.h"
#include "appmanager.h"
#include "component.h"

using namespace geeL::memory;

namespace geeL {
	
	bool Component::isActive() const {
		return active;
	}
	
	void Component::setActive(bool active) {
		this->active = active;
	}
	
	void* Component::operator new(size_t size) {
		Memory& memory = ApplicationManager::getCurrentMemory();
		return memory.allocate(size);
	}

	void Component::operator delete(void * pointer) {
		Memory& memory = ApplicationManager::getCurrentMemory();
		memory.deallocate(pointer);
	}

	void Component::setSceneObject(SceneObject& object) {
		sceneObject = &object;
	}

}