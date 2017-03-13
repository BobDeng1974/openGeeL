#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "../../dependencies/assimp/scene.h"

namespace geeL {

	class AlgebraHelper {

	public:

		inline static bool equals(const glm::vec3& v1, const glm::vec3& v2) {
			return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
		}

		inline static glm::vec3 lerp(const glm::vec3& v1, const glm::vec3& v2, float factor) {
			return v1 * factor + v2 * (1.f - factor);
		}

		inline static glm::vec3 quatToEuler(float x, float y, float z, float w) {
			float nx = atan2(2.f * (x * y + z * w), 1.f - 2.f * (y * y + z * z));
			float ny = asin(2.f * (x * z - w * y));
			float nz = atan2(2.f * (x * w + y * z), 1.f - 2.f * (z * z + w * w));

			return glm::vec3(nx, ny, nz);
		}

		inline static glm::mat4 convertMatrix(aiMatrix4x4& from) {
			glm::mat4 matrix;

			matrix[0][0] = from.a1;
			matrix[1][0] = from.a2;
			matrix[2][0] = from.a3;
			matrix[3][0] = from.a4;

			matrix[0][1] = from.b1;
			matrix[1][1] = from.b2;
			matrix[2][1] = from.b3;
			matrix[3][1] = from.b4;

			matrix[0][2] = from.c1;
			matrix[1][2] = from.c2;
			matrix[2][2] = from.c3;
			matrix[3][2] = from.c4;

			matrix[0][3] = from.d1;
			matrix[1][3] = from.d2;
			matrix[2][3] = from.d3;
			matrix[3][3] = from.d4;

			return matrix;
		}

	};
}

#endif

