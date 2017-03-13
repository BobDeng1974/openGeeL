#include <geometric.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../utility/vector3.h"
#include "transform.h"
#include <iostream>

using namespace std;
using glm::normalize;

namespace geeL {

	unsigned int idCounter = 0;

	Transform::Transform(Transform* parent) : parent(parent) {
		matrix = mat4();
		isStatic = true;

		id = idCounter;
		idCounter++;
	}

	Transform::Transform(mat4& matrix, Transform* parent) : matrix(matrix) {
		isStatic = true;
		id = idCounter;
		idCounter++;
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

		id = idCounter;
		idCounter++;
	}

	Transform::~Transform() {
		for (auto it = children.begin(); it != children.end(); it++)
			delete *it;
	}


	void Transform::setPosition(vec3 position) {
		if (!AlgebraHelper::equals(this->position, position)) {
			vec3 translation = position - this->position;
			this->position = position;

			matrix = glm::translate(matrix, translation);
			onChange();
		}
	}

	void Transform::setRotation(vec3 rotation) {
		if (!AlgebraHelper::equals(this->rotation, rotation)) {
			float pitch = rotation.x - this->rotation.x;
			float yaw = rotation.y - this->rotation.y;
			float roll = rotation.z - this->rotation.z;

			this->rotation += vec3(pitch, yaw, roll);

			matrix = glm::rotate(matrix, pitch, vec3(1.f, 0.f, 0.f));
			matrix = glm::rotate(matrix, yaw, vec3(0.f, 1.f, 0.f));
			matrix = glm::rotate(matrix, roll, vec3(0.f, 0.f, 1.f));

			updateDirections();
			onChange();
		}
	}

	void Transform::setScale(vec3 scaling) {
		if (!AlgebraHelper::equals(this->scaling, scaling)) {
			vec3 scalar = scaling - this->scaling;
			this->scaling = scaling;

			matrix = glm::scale(matrix, scalar);
			onChange();
		}
	}

	void Transform::translate(vec3 translation) {
		position += translation;
		matrix = glm::translate(matrix, translation);
		onChange();
	}

	void Transform::rotate(vec3 axis, float angle) {
		float newPitch = axis.x * angle;
		float newYaw = axis.y * angle;
		float newRoll = axis.z * angle;

		rotation += vec3(newPitch, newYaw, newRoll);
		matrix = glm::rotate(matrix, glm::radians(angle), axis);

		updateDirections();
		onChange();
	}

	void Transform::scale(vec3 scalar) {
		scaling += scalar;
		matrix = glm::scale(matrix, scalar);
		onChange();
	}

	mat4 Transform::lookAt() const {
		return glm::lookAt(position, position + forward, up);
	}

	list<Transform*>::iterator Transform::childrenStart() {
		return children.begin();
	}

	list<Transform*>::iterator Transform::childrenEnd() {
		return children.end();
	}

	const Transform* Transform::GetParent() const {
		return parent;
	}

	Transform& Transform::AddChild(const Transform& child) {
		children.push_back(new Transform(child));
		return *children.back();
	}

	Transform& Transform::AddChild(Transform* child) {
		children.push_back(child);
		return *children.back();
	}

	void Transform::RemoveChild(Transform& child) {
		children.remove(&child);
	}

	void Transform::ChangeParent(Transform& newParent) {
		parent->RemoveChild(*this);
		parent = &newParent;
	}

	void Transform::computeMatrix() {
		if (parent != nullptr)
			matrix = matrix * parent->matrix;

		for (auto it = children.begin(); it != children.end(); it++) {
			Transform& transform = **it;
			transform.computeMatrix();
		}

		onChange();
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

	unsigned int Transform::getID() const {
		return id;
	}

	void Transform::addChangeListener(function<void(const Transform&)> listener) {
		changeListener.push_back(listener);
	}

	void Transform::removeChangeListener(function<void(const Transform&)> listener) {
		//TODO: implement this
	}

	void Transform::onChange() {
		for (auto it = changeListener.begin(); it != changeListener.end(); it++) {
			auto func = *it;
			func(*this);
		}
	}

	const string& Transform::getName() const {
		return name;
	}

	void Transform::setName(string& name) {
		this->name = name;
	}

}