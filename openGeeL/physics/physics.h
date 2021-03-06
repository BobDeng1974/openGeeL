#ifndef PHYSICS_H
#define PHYSICS_H

#include <vec3.hpp>
#include "rigidbody.h"

namespace geeL {

	struct RigidbodyProperties;
	class Model;
	class Rigidbody;
	class Transform;

	//Container that stores information about successfull raycasts
	struct RayCastHit {

	public:
		glm::vec3 point;
		glm::vec3 normal;
		bool hit = false;

		operator bool() const {
			return hit;
		}

	};


	//Class that computes physical properties of objects in scene
	class Physics {
		
	public:
		virtual ~Physics() {}

		virtual void update() = 0;

		virtual void intersect(glm::vec3 start, glm::vec3 end, RayCastHit& hit) const = 0;

		virtual void addSphere(float radius, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addPlane(const glm::vec3 normal, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addBox(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addCylinder(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addCapsule(float radius, float height, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addMesh(const Model& model, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addStaticMesh(const Model& model, Transform& transform, RigidbodyProperties properties) = 0;

	};


	//Physics class that computes no physics. Intended for use as default value
	class NoPhysics : public Physics {

	public:
		virtual void update() {}

		virtual void intersect(glm::vec3 start, glm::vec3 end, RayCastHit& hit) const {}

		virtual void addSphere(float radius, Transform& transform, RigidbodyProperties properties) {}
		virtual void addPlane(const glm::vec3 normal, Transform& transform, RigidbodyProperties properties) {}
		virtual void addBox(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties) {}
		virtual void addCylinder(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties) {}
		virtual void addCapsule(float radius, float height, Transform& transform, RigidbodyProperties properties) {}
		virtual void addMesh(const Model& model, Transform& transform, RigidbodyProperties properties) {}
		virtual void addStaticMesh(const Model& model, Transform& transform, RigidbodyProperties properties) {}

	};


}

#endif

