#include "transform.h"
#include <geometric.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>

using glm::normalize;

namespace geeL {

	Transform::Transform(Transform* parent) : parent(parent) {
		matrix = mat4();
		isStatic = true;
	}

	Transform::Transform(vec3 position, vec3 rotation, vec3 scaling, Transform* parent) 
		: position(position), rotation(rotation), scaling(scaling), parent(parent) {
		
		matrix = mat4();
		isStatic = true;

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

	Transform::~Transform() {}

	void Transform::translate(vec3 translation) {
		position += translation;
		matrix = glm::translate(matrix, translation);
	}

	void Transform::rotate(vec3 axis, float angle) {
		float newPitch = axis.x * angle;
		float newYaw = axis.y * angle;
		float newRoll = axis.z * angle;

		rotation += vec3(newPitch, newYaw, newRoll);
		matrix = glm::rotate(matrix, glm::radians(angle), axis);

		updateDirections();
	}

	void Transform::scale(vec3 scalar) {
		scaling += scalar;
		matrix = glm::scale(matrix, scalar);
	}

	mat4 Transform::lookAt() const {
		return glm::lookAt(position, position + forward, up);
	}

	list<Transform>::iterator Transform::childrenStart() {
		return children.begin();
	}

	list<Transform>::iterator Transform::childrenEnd() {
		return children.end();
	}

	const Transform* Transform::GetParent() const {
		return parent;
	}

	Transform& Transform::AddChild(Transform child) {
		children.push_back(child);
		return children.back();
	}

	void Transform::RemoveChild(Transform& child) {
		children.remove(child);
	}

	void Transform::ChangeParent(Transform& newParent) {
		parent->RemoveChild(*this);
		parent = &newParent;
	}

	void Transform::computeMatrix() {
		if (parent != nullptr)
			matrix = matrix * parent->matrix;

		for (list<Transform>::iterator it = children.begin(); it != children.end(); it++) {
			Transform& transform = *it;
			transform.computeMatrix();
		}
	}

	bool Transform::operator==(const Transform& b) const {
		return this == &b;
	}

	bool Transform::operator!=(const Transform& b) const {
		return this != &b;
	}

	void Transform::updateDirections() {
		right =   normalize(vec3(matrix[0][0], matrix[0][1], matrix[0][2]));
		forward = normalize(vec3(matrix[1][0], matrix[1][1], matrix[1][2]));
		up =      normalize(vec3(matrix[2][0], matrix[2][1], matrix[2][2]));
	}

}