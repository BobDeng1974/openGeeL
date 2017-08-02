#ifndef  COMPONENT_H
#define  COMPONENT_H

#include "inputmanager.h"

namespace geeL {

	class SceneObject;

	class Component {
		friend class SceneObject;

	protected:
		SceneObject* sceneObject;

		void setSceneObject(SceneObject& object);

		virtual void init() {}
		virtual void update(Input& input) {}
		virtual void lateUpdate() {}

	};

	template<class Object>
	class GenericComponent : public Component {

	protected:
		Object* genericObject;

		void setSceneObject(Object& object);

	};


	inline void Component::setSceneObject(SceneObject& object) {
		sceneObject = &object;
	}

	template<class Object>
	inline void GenericComponent<Object>::setSceneObject(Object& object) {
		static_assert(std::is_base_of<SceneObject, Object>::value, "Given class is not a scene object");

		genericObject = &object;
	}

}

#endif
