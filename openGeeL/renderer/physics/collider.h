#ifndef COLLIDER_H
#define COLLIDER_H

#include <vec3.hpp>

class btCollisionShape;

namespace geeL {

	class Model;

	//Objects that defines a collision shape
	class Collider {

	public:
		~Collider();

		btCollisionShape* getShape() const;

	protected:
		Collider();

		btCollisionShape* shape;

	};


	class SphereCollider : public Collider {

	public:
		SphereCollider(float radius);

	};


	class PlaneCollider : public Collider {

	public:
		PlaneCollider(const glm::vec3& normal);

	};


	class BoxCollider : public Collider {

	public:
		BoxCollider(const glm::vec3& magnitude);

	};


	class CapsuleCollider : public Collider {

	public:
		CapsuleCollider(float radius, float height);

	};


	class CylinderCollider : public Collider {

	public:
		CylinderCollider(const glm::vec3& magnitude);

	};


	class MeshCollider : public Collider {

	public:
		MeshCollider(const Model& model);

	};

}

#endif
