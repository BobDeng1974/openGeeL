#include <vec3.hpp>
#include "inputmanager.h"
#include "sceneobject.h"
#include "luatypes.h"

namespace geeL {
namespace lua {

	void LUATypes::addVectorType(sol::state& state) {
		state.new_usertype<glm::vec3>("vec3",
			sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
			"x", &glm::vec3::x,
			"y", &glm::vec3::y,
			"z", &glm::vec3::z

			);
	}

	void LUATypes::addTransformType(sol::state& state) {
		state.new_usertype<Transform>("Transform",
			sol::constructors<>(),
			"position",		sol::property(&Transform::getPosition, &Transform::setPosition),
			"rotation",		sol::property(&Transform::getEulerAngles, &Transform::setEulerAngles),
			"scaling",		sol::property(&Transform::getScaling, &Transform::setScaling),
			"forward",		sol::property(&Transform::getForwardDirection, &Transform::setForward),
			"up",			&Transform::getUpDirection,
			"right",		&Transform::getRightDirection,

			"translate",	&Transform::translate,
			"rotate",		&Transform::rotate,
			"scale",		&Transform::scale

			);
	}

	void LUATypes::addSceneObjectType(sol::state& state) {
		state.new_usertype<SceneObject>("SceneObject",
			sol::constructors<>(),
			"transform", &SceneObject::getTransform,
			"active", sol::property(&SceneObject::isActive, &SceneObject::setActive)
			
			);

	}

	void LUATypes::addInputType(sol::state& state) {
		state.new_usertype<Input>("Input",
			sol::constructors<>(),
			"getKey",				&Input::getKey,
			"getKeyDown",			&Input::getKeyDown,
			"getKeyUp",				&Input::getKeyUp,
			"getKeyHold",			&Input::getKeyHold,
			"getButton",			&Input::getButton,
			"getButtonDown",		&Input::getButtonDown,
			"getButtonUp",			&Input::getButtonUp,
			"getButtonHold",		&Input::getButtonHold,
			"getMouseKey",			&Input::getMouseKey,
			"getMouseX",			&Input::getMouseX,
			"getMouseY",			&Input::getMouseY,
			"getMouseXNorm",		&Input::getMouseXNorm,
			"getMouseYNorm",		&Input::getMouseYNorm,
			"getMouseXOffset",		&Input::getMouseXOffset,
			"getMouseYOffset",		&Input::getMouseYOffset,
			"getMouseScroll",		&Input::getMouseScroll,
			"getMouseScrollOffset", &Input::getMouseScrollOffset

			);
	}

}
}
