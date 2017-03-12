#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>
#include <vec3.hpp>

namespace geeL {

	class Vector {

	public:

		inline static bool equals(const glm::vec3& v1, const glm::vec3& v2) {
			return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
		}

		inline static glm::vec3 quatToEuler(float x, float y, float z, float w) {
			float nx = atan2(2.f * (x * y + z * w), 1.f - 2.f * (y * y + z * z));
			float ny = asin(2.f * (x * z - w * y));
			float nz = atan2(2.f * (x * w + y * z), 1.f - 2.f * (z * z + w * w));

			return glm::vec3(nx, ny, nz);
		}

	};
}

#endif

