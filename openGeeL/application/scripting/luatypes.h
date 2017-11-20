#ifndef LUATYPES_H
#define LUATYPES_H

#include "sol.hpp"
#include "transformation/transform.h"

namespace geeL {
namespace lua {

	class LUATypes {

	public:
		static void addVectorType(sol::state& state);
		static void addTransformType(sol::state& state);
		static void addSceneObjectType(sol::state& state);

		static void addInputType(sol::state& state);

	};

}
}

#endif
