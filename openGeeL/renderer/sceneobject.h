#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <string>
#include <list>
#include <memory>

namespace geeL {

	class Component;
	class Transform;

	class SceneObject {

	public:
		Transform& transform;

		SceneObject(Transform& transform, std::string name = "Scene Object");

		//Update object and all its components
		virtual void update();

		//Add and init new component to scene object that will the be updated automatically
		void addComponent(std::shared_ptr<Component> component);

		bool isActive() const;
		void setActive(bool active);

		std::string getName() const;
		void setName(std::string name);

	private:
		bool active;
		std::string name;
		std::list<std::shared_ptr<Component>> components;

	};
}

#endif
