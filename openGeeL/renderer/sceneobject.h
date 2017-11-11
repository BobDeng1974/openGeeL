#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <string>
#include <list>
#include <memory>
#include "scripting/component.h"
#include "utility/property.h"

namespace geeL {

	class Component;
	class InputManager;
	class Transform;

	class SceneObject {

	public:
		Transform& transform;

		SceneObject(Transform& transform, const std::string& name = "Scene Object");
		virtual ~SceneObject();

		//Update object and all its components
		virtual void update(Input& input);

		//Update objects and all its components after transformation changes
		virtual void lateUpdate();

		//Add and init new empty component of given type to scene object that will be updated automatically
		template<class T>
		T& addComponent();

		//Add given component to scene object that will be updated automatically
		template<class T>
		T& addComponent(T&& component);

		//Create new component with given paremeters and add it to scene object
		template<class T, typename ...Args>
		T& addComponent(const Args& ...args);

		virtual bool isActive() const;
		virtual void setActive(bool active);
		void addStatusListener(std::function<void(SceneObject&, bool status)> listener);

		std::string getName() const;
		void setName(const std::string& name);

	protected:
		Property<bool> active;
		std::string name;
		std::list<Component*> components;

	};


	template<class T>
	inline T& SceneObject::addComponent(T&& component) {
		static_assert(std::is_base_of<Component, T>::value, "Given class is not a component");

		T* comp = new T(std::move(component));
		comp->setSceneObject(*this);
		comp->init();
		components.push_back(comp);

		return *comp;
	}

	template<class T>
	inline T& SceneObject::addComponent() {
		static_assert(std::is_base_of<Component, T>::value, "Given class is not a component");

		T* comp = new T();
		comp->setSceneObject(*this);
		comp->init();
		components.push_back(comp);

		return *comp;
	}

	template<class T, typename ...Args>
	inline T& SceneObject::addComponent(const Args& ...args) {
		static_assert(std::is_base_of<Component, T>::value, "Given class is not a component");

		T* comp = new T(args...);
		comp->setSceneObject(*this);
		comp->init();
		components.push_back(comp);

		return *comp;
	}

}

#endif
