#include <iostream>
#include "sol.hpp"
#include "sceneobject.h"
#include "luatypes.h"
#include "luacomp.h"

using namespace geeL::lua;

namespace geeL {

	LUAComponent::LUAComponent(const std::string& scriptPath) {
		state = new sol::state();

		try {
			state->script_file(scriptPath);
		}
		catch (sol::error e) {
			throw e.what();
		}

		LUATypes::addInputType(*state);
		LUATypes::addVectorType(*state);
		LUATypes::addTransformType(*state);
		LUATypes::addSceneObjectType(*state);
	}

	LUAComponent::~LUAComponent() {
		delete state;
	}

	void LUAComponent::setSceneObject(SceneObject& object) {
		Component::setSceneObject(object);

		(*state)["sceneObject"] = sceneObject;
		(*state)["transform"] = &sceneObject->transform;
	}

	void LUAComponent::init() {
		execute("init");
	}

	void LUAComponent::update(Input& input) {
		(*state)["input"] = &input;
		execute("update");
	}

	void LUAComponent::lateUpdate() {
		execute("lateUpdate");
	}

	void LUAComponent::execute(const std::string & functionName) {
		sol::function f = (*state)[functionName];
		if (f.valid()) f();
	}

}