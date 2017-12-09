#include <algorithm>
#include <iostream>
#include "transformation/transform.h"
#include "memory/memory.h"
#include "appmanager.h"
#include "sceneobject.h"

using namespace std;
using namespace geeL::memory;

namespace geeL {

	SceneObject::SceneObject(Transform& transform, const std::string& name)
		: transform(transform)
		, name(name)
		, active(true) {}

	SceneObject::~SceneObject() {
		for_each(components.begin(), components.end(), [&](Component* comp) {
			delete comp;
		});
	}


	void SceneObject::update(Input& input) {
		for_each(components.begin(), components.end(), [&](Component* comp) {
			comp->update(input);
		});
	}

	void SceneObject::lateUpdate() {
		for_each(components.begin(), components.end(), [&](Component* comp) {
			comp->lateUpdate();
		});
	}

	bool SceneObject::isActive() const {
		return active;
	}

	void SceneObject::setActive(bool active) {
		if (this->active != active) {
			if (!transform.isStatic)
				this->active = active;
			else
				std::cout << "Can't de-/activate static objects\n";
		}
	}

	void SceneObject::addStatusListener(std::function<void(SceneObject&, bool status)> listener) {
		active.addListener([this, listener](const bool& value) {
			listener(*this, value);
		});
	}

	std::string SceneObject::getName() const {
		return name;
	}

	void SceneObject::setName(const std::string& name) {
		this->name = std::move(name);
	}

	Transform& SceneObject::getTransform() {
		return transform;
	}


	void* SceneObject::operator new(size_t size) {
		Memory& memory = ApplicationManager::getCurrentMemory();
		return memory.allocate(size);
	}

	void SceneObject::operator delete(void* pointer) {
		Memory& memory = ApplicationManager::getCurrentMemory();
		//memory.deallocate(pointer);
	}

}