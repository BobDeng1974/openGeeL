#include <geometric.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>
#include <iostream>
#include <glm.hpp>
#include "utility/vectorextension.h"
#include "transform.h"

using namespace std;
using glm::normalize;
using namespace glm;

namespace geeL {

	unsigned int idCounter = 0;

	Transform::Transform() : matrix(mat4()), isStatic(true), parent(nullptr), status(TransformUpdateStatus::None) {
		id = idCounter;
		idCounter++;
	}

	Transform::Transform(mat4& matrix, bool isStatic) : matrix(matrix), isStatic(isStatic), parent(nullptr) {
		setMatrix(matrix);
		status = TransformUpdateStatus::None; //Set status here since 'setMatrix' already changed it

		id = idCounter;
		idCounter++;
	}

	Transform::Transform(vec3 position, glm::quat rotation, vec3 scaling, bool isStatic)
		: position(position), rotation(rotation), scaling(scaling), isStatic(isStatic), parent(nullptr), status(TransformUpdateStatus::None) {

		translationMatrix = glm::translate(glm::mat4(1.f), position);
		rotationMatrix = glm::toMat4(this->rotation);
		scaleMatrix = glm::scale(glm::mat4(1.f), scaling);
		updateDirections();
		resetMatrix();

		id = idCounter;
		idCounter++;
	}

	Transform::Transform(vec3 position, vec3 rotation, vec3 scaling, bool isStatic)
		: position(position), scaling(scaling), isStatic(isStatic), parent(nullptr), status(TransformUpdateStatus::None) {
		
		setEulerAnglesInternal(rotation);
		translationMatrix = glm::translate(glm::mat4(1.f), position);
		rotationMatrix = glm::toMat4(this->rotation);
		scaleMatrix = glm::scale(glm::mat4(1.f), scaling);
		updateDirections();
		resetMatrix();

		id = idCounter;
		idCounter++;
	}

	Transform::Transform(const Transform& transform) : 
		position(transform.position), rotation(transform.rotation), scaling(transform.scaling), 
		forward(transform.forward), up(transform.up), right(transform.right),
		translationMatrix(transform.translationMatrix), rotationMatrix(transform.rotationMatrix), scaleMatrix(transform.scaleMatrix), 
		isStatic(transform.isStatic), parent(transform.parent), status(TransformUpdateStatus::None), name(transform.name) {

		resetMatrix();

		id = idCounter;
		idCounter++;
	}


	Transform::~Transform() {
		for (auto it = children.begin(); it != children.end(); it++)
			delete *it;
	}



	glm::vec3 Transform::getPosition() const {
		return position;
	}

	glm::quat Transform::getRotation() const {
		return rotation;
	}

	glm::vec3 Transform::getScaling() const {
		return scaling;
	}


	glm::vec3 Transform::getForwardDirection() const {
		return forward;
	}

	glm::vec3 Transform::getRightDirection() const {
		return right;
	}

	glm::vec3 Transform::getUpDirection() const {
		return up;
	}

	const glm::vec3& Transform::getPosition() {
		return position;
	}

	const glm::quat& Transform::getRotation() {
		return rotation;
	}

	const glm::vec3& Transform::getScaling() {
		return scaling;
	}

	const glm::vec3& Transform::getForwardDirection() {
		return forward;
	}

	const glm::vec3& Transform::getRightDirection() {
		return right;
	}

	const glm::vec3& Transform::getUpDirection() {
		return up;
	}

	const glm::mat4& Transform::getMatrix() {
		return matrix;
	}

	
	vec3 Transform::getEulerAngles() {
		return glm::eulerAngles(rotation);
	}


	void Transform::setPosition(const vec3& position) {
		if (!isStatic && !VectorExtension::equals(this->position, position)) {
			this->position = position;

			translationMatrix[3][0] = position.x;
			translationMatrix[3][1] = position.y;
			translationMatrix[3][2] = position.z;

			status = TransformUpdateStatus::NeedsUpdate;
		}
	}

	void Transform::setRotation(const glm::quat& quaternion) {
		if (!isStatic && !VectorExtension::equals(rotation, quaternion)) {
			rotation = quaternion;
			rotationMatrix = glm::toMat4(rotation);
			updateDirections();

			status = TransformUpdateStatus::NeedsUpdate;
		}
	}

	void Transform::setForward(const vec3& value) {
		forward = value;
		right = glm::cross(up, value);
		up = glm::cross(value, right);

		rotationMatrix[0][0] = right.x;
		rotationMatrix[0][1] = right.y;
		rotationMatrix[0][2] = right.z;

		rotationMatrix[1][0] = forward.x;
		rotationMatrix[1][1] = forward.y;
		rotationMatrix[1][2] = forward.z;

		rotationMatrix[2][0] = up.x;
		rotationMatrix[2][1] = up.y;
		rotationMatrix[2][2] = up.z;

		rotation = glm::toQuat(rotationMatrix);

		status = TransformUpdateStatus::NeedsUpdate;
	}

	void Transform::setMatrix(const mat4& matrix) {
		if (isStatic) return;

		//Extract position from matrix
		position.x = matrix[3][0];
		position.y = matrix[3][1];
		position.z = matrix[3][2];

		mat3 m = mat3(matrix);

		//Extract scale and normalize span vectors
		vec3 r = vec3(m[0][0], m[0][1], m[0][2]);
		vec3 f = vec3(m[1][0], m[1][1], m[1][2]);
		vec3 u = vec3(m[2][0], m[2][1], m[2][2]);

		scaling.x = glm::length(r);
		scaling.y = glm::length(f);
		scaling.z = glm::length(u);

		m[0][0] /= scaling.x;
		m[0][1] /= scaling.x;
		m[0][2] /= scaling.x;

		m[1][0] /= scaling.y;
		m[1][1] /= scaling.y;
		m[1][2] /= scaling.y;

		m[2][0] /= scaling.z;
		m[2][1] /= scaling.z;
		m[2][2] /= scaling.z;

		//Convert remaining rotation matrix to quaternion
		rotation = glm::quat_cast(m);

		translationMatrix = glm::translate(glm::mat4(1.f), position);
		rotationMatrix = glm::toMat4(this->rotation); //Maybe use m here directly
		scaleMatrix = glm::scale(glm::mat4(1.f), scaling);
		updateDirections();

		status = TransformUpdateStatus::NeedsUpdate;
	}

	
	void Transform::setEulerAnglesInternal(const vec3& eulerAngles) {
		rotation = glm::quat(vec3(glm::radians(eulerAngles.x),
			glm::radians(eulerAngles.y),
			glm::radians(eulerAngles.z)));

		rotationMatrix = glm::toMat4(rotation);
		updateDirections();

	}

	void Transform::setEulerAngles(const vec3& eulerAngles) {
		if (!isStatic && !VectorExtension::equals(glm::eulerAngles(rotation), eulerAngles)) {
			setEulerAnglesInternal(eulerAngles);
			status = TransformUpdateStatus::NeedsUpdate;
		}
	}

	void Transform::setScaling(const vec3& scaling) {
		if (!isStatic && !VectorExtension::equals(this->scaling, scaling)) {
			this->scaling = scaling;

			scaleMatrix[0][0] = scaling.x;
			scaleMatrix[1][1] = scaling.y;
			scaleMatrix[2][2] = scaling.z;

			status = TransformUpdateStatus::NeedsUpdate;
		}
	}


	void Transform::translate(const vec3& translation) {
		if (isStatic) return;

		position += translation;

		translationMatrix[3][0] += translation.x;
		translationMatrix[3][1] += translation.y;
		translationMatrix[3][2] += translation.z;

		status = TransformUpdateStatus::NeedsUpdate;
	}

	void Transform::rotate(const vec3& axis, float angle) {
		if (isStatic) return;

		glm::quat localRotation;
		float angleHalf = angle * 0.5f;
		float sinAngleHalf = sinf(angleHalf);

		localRotation.x = axis.x * sinAngleHalf;
		localRotation.y = axis.y * sinAngleHalf;
		localRotation.z = axis.z * sinAngleHalf;
		localRotation.w = cosf(angleHalf);

		rotation = localRotation * rotation;

		rotationMatrix = glm::toMat4(rotation);
		updateDirections();

		status = TransformUpdateStatus::NeedsUpdate;
	}

	void Transform::scale(const vec3& scalar) {
		if (isStatic) return;

		scaling += scalar;

		scaleMatrix[0][0] += scalar.x;
		scaleMatrix[1][1] += scalar.y;
		scaleMatrix[2][2] += scalar.z;

		status = TransformUpdateStatus::NeedsUpdate;
	}

	mat4 Transform::lookAt() {
		return glm::lookAt(position, position + forward, up);
	}

	void Transform::updateDirections() {
		right = vec3(rotationMatrix[0][0], rotationMatrix[0][1], rotationMatrix[0][2]);
		forward = vec3(rotationMatrix[1][0], rotationMatrix[1][1], rotationMatrix[1][2]);
		up = vec3(rotationMatrix[2][0], rotationMatrix[2][1], rotationMatrix[2][2]);
	}
	

	void Transform::resetMatrix() {
		matrix = translationMatrix * rotationMatrix * scaleMatrix;

		if (parent != nullptr)
			matrix = parent->matrix * matrix;

		onChange();
	}

	void Transform::update() {
		//Update transformation matrix if position, rotation or scale has
		//been updated during the current cycle (since last 'update' call)
		if (status == TransformUpdateStatus::NeedsUpdate 
			//Or update matrix if the parent transform has changed
			|| (parent != nullptr && parent->hasUpdated())) {
			
			resetMatrix();
			status = TransformUpdateStatus::HasUpdated;
		}
		else
			status = TransformUpdateStatus::None;
		
		//Update all children in transform tree
		for (auto it = children.begin(); it != children.end(); it++) {
			Transform& transform = **it;
			transform.update();
		}
	}

	bool Transform::hasUpdated() const {
		return status == TransformUpdateStatus::HasUpdated;
	}

	bool Transform::operator==(const Transform& b) const {
		return this == &b;
	}

	bool Transform::operator!=(const Transform& b) const {
		return this != &b;
	}

	Transform& Transform::operator=(const Transform& other) {
		if (this != &other)
			*this = std::move(Transform(other));

		return *this;
	}

	unsigned int Transform::getID() const {
		return id;
	}

	void Transform::addChangeListener(function<void(const Transform&)> listener) {
		changeListener.push_back(listener);
	}


	void Transform::onChange() {
		for (auto it = changeListener.begin(); it != changeListener.end(); it++) {
			auto func = *it;
			func(*this);
		}
	}

	Transform* Transform::GetParent() {
		return parent;
	}

	void Transform::iterateChildren(std::function<void(Transform&)> function) {
		for (auto it(children.begin()); it != children.end(); it++)
			function(**it);
	}


	Transform& Transform::AddChild(const Transform& child) {
		return AddChild(new Transform(child));
	}

	Transform& Transform::AddChild(Transform* child) {
		children.push_back(child);
		child->ChangeParent(*this);

		return *child;
	}

	void Transform::RemoveChild(Transform& child) {
		children.remove(&child);
	}

	void Transform::ChangeParent(Transform& newParent) {
		if (&newParent != parent) {
			if (parent != nullptr)
				parent->RemoveChild(*this);

			parent = &newParent;
			status = TransformUpdateStatus::NeedsUpdate;

			//TODO: change local transform to match old local 
			//transform relative to old parent
		}
	}


	const string& Transform::getName() {
		return name;
	}

	void Transform::setName(const string& name) {
		this->name = name;
	}


	std::string Transform::toString() const {
		return "Transform " + name + ": " + std::to_string(id) + "\n"
			+ "--Position: " + VectorExtension::vectorString(position) + "\n"
			+ "--Rotation: " + VectorExtension::vectorString(glm::eulerAngles(rotation)) + "\n"
			+ "--Scale:    " + VectorExtension::vectorString(scaling) + "\n"
			+ "--Forward:  " + VectorExtension::vectorString(forward) + "\n"
			+ "--Up:       " + VectorExtension::vectorString(up) + "\n"
			+ "--Right:    " + VectorExtension::vectorString(right) + "\n";
	}

}