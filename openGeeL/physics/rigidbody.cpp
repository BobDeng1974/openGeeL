#include <btBulletDynamicsCommon.h>
#include "transformation/transform.h"
#include "motionstate.h"
#include "collider.h"
#include "rigidbody.h"

namespace geeL {

	Rigidbody::Rigidbody(Collider* collider, Transform& transform, RigidbodyProperties properties) 
		: collider(collider), transform(transform), properties(properties) {

		btCollisionShape* shape = collider->getShape();

		if(properties.isKinematic)
			motionState = new KinematicMotionState(transform);
		else
			motionState = new MotionState(transform);

		btScalar bmass = properties.mass;
		btVector3 fallInertia(0, 0, 0);
		shape->calculateLocalInertia(bmass, fallInertia);

		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(bmass, motionState, shape, fallInertia);
		body = new btRigidBody(fallRigidBodyCI);

		if (properties.isKinematic) {
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			body->setActivationState(DISABLE_DEACTIVATION);
		}
	}

	Rigidbody::~Rigidbody() {
		delete collider;
		delete motionState;
		delete body;
	}


	void Rigidbody::update() {
		motionState->update();
	}

	btRigidBody& Rigidbody::getRigidbody() const {
		return *body;
	}

}