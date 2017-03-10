#ifndef PHYSICS_H
#define PHYSICS_H

#include <vec3.hpp>

namespace geeL {

	struct RigidbodyProperties;
	class Model;
	class Rigidbody;
	class Transform;

	//Class that computes physical properties of objects in scene
	class Physics {
		
	public:
		virtual void update() = 0;

		virtual void addSphere(float radius, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addPlane(const glm::vec3 normal, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addBox(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addCylinder(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addCapsule(float radius, float height, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addMesh(const Model& model, Transform& transform, RigidbodyProperties properties) = 0;

	};
}

#endif

