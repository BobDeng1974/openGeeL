#include <algorithm>
#include "scripting/component.h"
#include "sceneobject.h"

using namespace std;

namespace geeL {

	SceneObject::SceneObject(Transform& transform) : transform(transform), active(true) {}


	void SceneObject::update() {
		for_each(components.begin(), components.end(), [&](shared_ptr<Component> comp) {
			comp->update();
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

}