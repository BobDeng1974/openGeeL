#include <algorithm>
#include "scripting/component.h"
#include "sceneobject.h"

using namespace std;

namespace geeL {

	SceneObject::SceneObject(Transform& transform, std::string name) 
		: transform(transform), name(name), active(true) {}


	void SceneObject::update() {
		for_each(components.begin(), components.end(), [&](shared_ptr<Component> comp) {
			comp->update();
		});
	}

	void SceneObject::lateUpdate() {
		for_each(components.begin(), components.end(), [&](shared_ptr<Component> comp) {
			comp->lateUpdate();
		});
	}

	void SceneObject::addComponent(shared_ptr<Component> component) {
		component->init();
		components.push_back(component);
	}

	bool SceneObject::isActive() const {
		return active;
	}

	void SceneObject::setActive(bool active) {
		this->active = active;
	}

	std::string SceneObject::getName() const {
		return name;
	}

	void SceneObject::setName(std::string name) {
		this->name = name;
	}

}