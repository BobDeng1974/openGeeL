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
		~SceneObject();

		//Update object and all its components
		virtual void update();

		//Update objects and all its components after transformation changes
		virtual void lateUpdate();

		//Add and init new empty component of given type to scene object that will be updated automatically
		template<class T>
		T& addComponent();

		//Add copy of given component to scene object that will be updated automatically
		Component& addComponent(const Component& component);

		bool isActive() const;
		void setActive(bool active);

		std::string getName() const;
		void setName(std::string name);

	protected:
		bool active;
		std::string name;
		std::list<Component*> components;

	};


	template<class T>
	inline T& SceneObject::addComponent() {
		T* comp = new T();

		if (dynamic_cast<Component*>(comp) != nullptr) {
			comp->init();
			components.push_back(comp);
		}
		else
			std::cout << "Given type has to be a component type\n";
	}

}

#endif
