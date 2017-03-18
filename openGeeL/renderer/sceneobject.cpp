#include <algorithm>
#include <iostream>
#include "scripting/component.h"
#include "sceneobject.h"

using namespace std;

namespace geeL {

	SceneObject::SceneObject(Transform& transform, std::string name) 
		: transform(transform), name(name), active(true) {}

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

	template<class T>
	T& SceneObject::addComponent() {
		T* comp = new T();

		if (dynamic_cast<Component*>(comp) != nullptr) {
			comp->init();
			components.push_back(comp);
		}
		else
			std::cout << "Given type has to be a component type\n";

	}

	Component&  SceneObject::addComponent(const Component& component) {
		Component* comp = new Component(component);
		comp->init();

		components.push_back(comp);
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