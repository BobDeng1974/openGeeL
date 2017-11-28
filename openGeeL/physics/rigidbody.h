#ifndef RIGIDBODY_H
#define RIGIDBODY_H

class btTransform;
class btRigidBody;
class btCollisionShape;

namespace geeL {

	class Collider;
	class MotionState;
	class Transform;

	//Container that holds rigidbodies properties
	struct RigidbodyProperties {

	public:
		float mass;
		bool isKinematic;

		RigidbodyProperties(float mass = 0.f, bool isKinematic = false);

	};

	//Represents physical properties of an object 
	class Rigidbody {

	public:
		Rigidbody(Collider* collider, Transform& transform, RigidbodyProperties properties);
		~Rigidbody();

		void update();
		btRigidBody& getRigidbody() const;

	private:
		Transform& transform;
		RigidbodyProperties properties;

		Collider* collider;
		btRigidBody* body;
		MotionState* motionState;

	};


	inline RigidbodyProperties::RigidbodyProperties(float mass, bool isKinematic)
		: mass(isKinematic ? 0.f : mass)
		, isKinematic(isKinematic) {}


}

#endif

