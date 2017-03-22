#include <algorithm>
#include <iostream>
#include "scripting/component.h"
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
		this->active = active;
	}

	std::string SceneObject::getName() const {
		return name;
	}

	void SceneObject::setName(const std::string& name) {
		this->name = std::move(name);
	}

}