#include <geometric.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../utility/algebrahelper.h"
#include "transform.h"

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
		
		resetMatrix();

		isStatic = true;
		id = idCounter;
		idCounter++;
	}


	Transform::~Transform() {
		for (auto it = children.begin(); it != children.end(); it++)
			delete *it;
	}


	void Transform::resetMatrix() {
		matrix = mat4();
		matrix = glm::translate(matrix, position);
		matrix = glm::rotate(matrix, rotation.x, vec3(1.f, 0.f, 0.f));
		matrix = glm::rotate(matrix, rotation.y, vec3(0.f, 1.f, 0.f));
		matrix = glm::rotate(matrix, rotation.z, vec3(0.f, 0.f, 1.f));
		matrix = glm::scale(matrix, scaling);

		updateDirections();
		onChange();
	}


	const glm::vec3& Transform::getPosition() const {
		return position;
	}

	const glm::vec3& Transform::getRotation() const {
		return rotation;
	}

	const glm::vec3& Transform::getScaling() const {
		return scaling;
	}

	const glm::vec3& Transform::getForwardDirection() const {
		return forward;
	}

	const glm::vec3& Transform::getRightDirection() const {
		return right;
	}

	const glm::vec3& Transform::getUpDirection() const {
		return up;
	}

	const glm::mat4& Transform::getMatrix() const {
		return matrix;
	}


	void Transform::setPosition(const vec3& position) {
		if (!AlgebraHelper::equals(this->position, position)) {
			this->position = position;
			resetMatrix();
		}
	}

	void Transform::setRotation(const vec3& rotation) {
		if (!AlgebraHelper::equals(this->rotation, rotation)) {
			this->rotation = rotation;
			resetMatrix();
		}
	}

	void Transform::setScaling(const vec3& scaling) {
		if (!AlgebraHelper::equals(this->scaling, scaling)) {
			this->scaling = scaling;
			resetMatrix();
		}
	}


	void Transform::translate(const vec3& translation) {
		position += translation;
		resetMatrix();
	}

	void Transform::rotate(const vec3& axis, float angle) {
		float newPitch = axis.x * angle;
		float newYaw = axis.y * angle;
		float newRoll = axis.z * angle;

		rotation += vec3(newPitch, newYaw, newRoll);
		resetMatrix();
	}

	void Transform::scale(const vec3& scalar) {
		scaling += scalar;
		resetMatrix();
	}

	mat4 Transform::lookAt() const {
		return glm::lookAt(position, position + forward, up);
	}

	void Transform::updateDirections() {
		right = normalize(vec3(matrix[0][0], matrix[0][1], matrix[0][2]));
		forward = normalize(vec3(matrix[1][0], matrix[1][1], matrix[1][2]));
		up = normalize(vec3(matrix[2][0], matrix[2][1], matrix[2][2]));
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