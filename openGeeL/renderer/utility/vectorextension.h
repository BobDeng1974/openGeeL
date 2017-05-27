#ifndef VECTOREXTENSION_H
#define VECTOREXTENSION_H

#include <cmath>
#include <vec3.hpp>
#include <string>
#include <gtc/quaternion.hpp>
#include <iostream>

namespace geeL {

	//Extension class that adds additional functionality to 3D vectors
	class VectorExtension {

	public:

		inline static bool equals(const glm::vec3& v1, const glm::vec3& v2) {
			return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
		}

		inline static bool equals(const glm::quat& q1, const glm::quat& q2) {
			return (q1.x == q2.x) && (q1.y == q2.y) && (q1.z == q2.z) && (q1.w == q2.w);
		}

		inline static void printVector(const glm::vec3 vector) {
			std::cout << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")\n";
		}

		inline static std::string vectorString(const glm::vec3 vector) {
			return "(" + std::to_string(vector.x) + ", " + std::to_string(vector.y) + ", " + std::to_string(vector.z) + ")";
		}

		inline static glm::vec3 lerp(const glm::vec3& v1, const glm::vec3& v2, float factor) {
			return v1 * factor + v2 * (1.f - factor);
		}

		inline static glm::vec3 quatToEuler(float x, float y, float z, float w) {
			glm::quat quat = glm::quat(w, x, y, z);
			return glm::eulerAngles(quat);
		}

	};
}

#endif

