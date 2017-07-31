#include <algorithm>
#include <iostream>
#include "transformation/transform.h"
#include "sceneobject.h"

using namespace std;

namespace geeL {

	SceneObject::SceneObject(Transform& transform, const std::string& name)
		: transform(transform), name(std::move(name)), active(true) {}

	SceneObject::~SceneObject() {
		for_each(components.begin(), components.end(), [&](Component* comp) {
			delete comp;
		});
	}


	void SceneObject::update() {
		for_each(components.begin(), components.end(), [&](Component* comp) {
			comp->update();
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

	std::string SceneObject::getName() const {
		return name;
	}

	void SceneObject::setName(const std::string& name) {
		this->name = std::move(name);
	}

}