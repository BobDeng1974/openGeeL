#ifndef LUACOMPONENT_H
#define LUACOMPONENT_H

#include <string>
#include "component.h"

namespace sol {
	class state;

}

namespace geeL {

	class LUAComponent : public Component {

	public:
		LUAComponent(const std::string& scriptPath);
		virtual ~LUAComponent();

		virtual void setSceneObject(SceneObject& object);

		virtual void init();
		virtual void update(Input& input);
		virtual void lateUpdate();

	private:
		sol::state* state;

		void execute(const std::string& functionName);

	};

}

#endif
