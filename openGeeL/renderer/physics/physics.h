#ifndef PHYSICS_H
#define PHYSICS_H

#include <vec3.hpp>

namespace geeL {

	struct RigidbodyProperties;
	class StaticModel;
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
		virtual void update() = 0;

		virtual void intersect(glm::vec3 start, glm::vec3 end, RayCastHit& hit) const = 0;

		virtual void addSphere(float radius, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addPlane(const glm::vec3 normal, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addBox(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addCylinder(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addCapsule(float radius, float height, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addMesh(const StaticModel& model, Transform& transform, RigidbodyProperties properties) = 0;
		virtual void addStaticMesh(const StaticModel& model, Transform& transform, RigidbodyProperties properties) = 0;

	};
}

#endif

