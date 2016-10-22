#include "scripting/component.h"
#include "sceneobject.h"

using namespace std;


namespace geeL {

	SceneObject::SceneObject(Transform& transform) : transform(transform) {}


	void SceneObject::update() {
		for (list<std::shared_ptr<Component>>::iterator it = components.begin(); it != components.end(); it++) {
			shared_ptr<Component> comp = *it;
			comp->update();
		}
	}

	void SceneObject::addComponent(std::shared_ptr<Component> component) {
		component->init();
		components.push_back(component);
	}

}