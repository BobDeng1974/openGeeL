#ifndef  COMPONENT_H
#define  COMPONENT_H

#include "inputmanager.h"

namespace geeL {

	class SceneObject;

	class Component {
		friend class SceneObject;

	protected:
		SceneObject* sceneObject;

		virtual void setSceneObject(SceneObject& object);

		virtual void init() {}
		virtual void update(Input& input) {}
		virtual void lateUpdate() {}

	};

	template<class Object>
	class GenericComponent : public Component {
		friend class SceneObject;

	protected:
		Object* genericObject;

		virtual void setSceneObject(SceneObject& object);

	};


	inline void Component::setSceneObject(SceneObject& object) {
		sceneObject = &object;
	}

	template<class Object>
	inline void GenericComponent<Object>::setSceneObject(SceneObject& object) {
		Component::setSceneObject(object);

		Object* obj = dynamic_cast<Object*>(&object);
		if (obj == nullptr)
			throw "Given scene object has wrong child type\n";

		genericObject = obj;
	}

}

#endif
