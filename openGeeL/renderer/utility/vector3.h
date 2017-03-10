#ifndef VECTOR3_H
#define VECTOR3_H

#include <vec3.hpp>

namespace geeL {

	class Vector {

	public:

		inline static bool equals(const glm::vec3& v1, const glm::vec3& v2) {
			return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
		}

	};
}

#endif

