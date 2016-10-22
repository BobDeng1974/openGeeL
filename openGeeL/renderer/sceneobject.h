#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <list>
#include <memory>

namespace geeL {

class Component;
class Transform;

class SceneObject {

public:
	Transform& transform;

	SceneObject(Transform& transform);

	//Update object and all its components
	virtual void update();

	//Add and init new component to scene object that will the be updated automatically
	void addComponent(std::shared_ptr<Component> component);

private:
	std::list<std::shared_ptr<Component>> components;


};

}

#endif
