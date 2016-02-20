#include "transform.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace geeL {

	Transform::Transform() {
		matrix = mat4();
		isStatic = true;
	}

	Transform::Transform(vec3 position, vec3 rotation, vec3 scaling) : position(position), rotation(rotation), scaling(scaling) {
		Transform();

		float pitch = rotation.x;
		float yaw = rotation.y;
		float roll = rotation.z;
		
		matrix = glm::scale(matrix, scaling);
		matrix = glm::rotate(matrix, pitch, vec3(1.f, 0.f, 0.f));
		matrix = glm::rotate(matrix, yaw, vec3(0.f, 1.f, 0.f));
		matrix = glm::rotate(matrix, roll, vec3(0.f, 0.f, 1.f));
		matrix = glm::translate(matrix, position);
		
		updateDirections();
	}

	Transform::~Transform() {
		for (size_t i = 0; i < children.size(); i++)
			children[i]->parent = nullptr;
	}

	void Transform::translate(vec3 translation) {
		position += translation;
		matrix = glm::translate(matrix, translation);
	}

	void Transform::rotate(vec3 axis, float angle) {

		float newPitch = axis.x * angle;
		float newYaw = axis.y * angle;
		float newRoll = axis.z * angle;

		rotation += vec3(newPitch, newYaw, newRoll);
		matrix = glm::rotate(matrix, angle, axis);

		updateDirections();
	}

	void Transform::scale(vec3 scalar) {
		scaling += scalar;
		matrix = glm::scale(matrix, scalar);
	}

	mat4 Transform::lookAt() const {
		return glm::lookAt(position, position + forward, up);
	}

	std::vector<Transform*>::iterator Transform::childrenStart() {
		return children.begin();
	}

	std::vector<Transform*>::iterator Transform::childrenEnd() {
		return children.end();
	}

	void Transform::computeMatrix() {
		if (parent != nullptr)
			matrix = matrix * parent->matrix;

		for (size_t i = 0; i < children.size(); i++)
			children[i]->computeMatrix();
	}

	void Transform::updateDirections() {
		forward = normalize(vec3(matrix[0][1], matrix[1][1], matrix[2][1]));
		up = normalize(vec3(matrix[0][2], matrix[1][2], matrix[2][2]));
		right = normalize(vec3(matrix[0][0], matrix[1][0], matrix[2][0]));
	}

}