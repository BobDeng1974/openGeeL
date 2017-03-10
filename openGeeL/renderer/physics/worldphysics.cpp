#include <btBulletDynamicsCommon.h>
#include "collider.h"
#include "rigidbody.h"
#include "worldphysics.h"
#include <iostream>

namespace geeL {

	WorldPhysics::WorldPhysics() {
		init();
	}

	WorldPhysics::~WorldPhysics() {

		for (auto it = rigidbodies.begin(); it != rigidbodies.end(); it++) {
			Rigidbody* body = *it;

			world->removeRigidBody(&body->getRigidbody());
			delete body;
		}

		for (auto it = kinematicRigidbodies.begin(); it != kinematicRigidbodies.end(); it++) {
			Rigidbody* body = *it;

			world->removeRigidBody(&body->getRigidbody());
			delete body;
		}

		delete broadPhase;
		delete collisionConfiguration;
		delete dispatcher;
		delete solver;

		delete world;
	}


	void WorldPhysics::init() {
		broadPhase = new btDbvtBroadphase();
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		solver = new btSequentialImpulseConstraintSolver;

		world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConfiguration);
		world->setGravity(btVector3(0, -10, 0));
	}

	void WorldPhysics::update() {
		world->stepSimulation(1.f / 30.f, 10);

		//Only update kinematic rigidbodies since their are the only ones that can be moved manually in application
		for (auto it = kinematicRigidbodies.begin(); it != kinematicRigidbodies.end(); it++) {
			Rigidbody& body = **it;
			
			body.update();
		}
	}

	void  WorldPhysics::addSphere(float radius, Transform& transform, RigidbodyProperties properties) {
		SphereCollider* coll = new SphereCollider(radius);
		addRigidbody(coll, transform, properties);
	}

	void  WorldPhysics::addPlane(const glm::vec3 normal, Transform& transform, RigidbodyProperties properties) {
		PlaneCollider* coll = new PlaneCollider(normal);
		addRigidbody(coll, transform, properties);
	}

	void WorldPhysics::addBox(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties) {
		BoxCollider* coll = new BoxCollider(magnitude);
		addRigidbody(coll, transform, properties);
	}

	void WorldPhysics::addCylinder(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties) {
		CylinderCollider* coll = new CylinderCollider(magnitude);
		addRigidbody(coll, transform, properties);
	}

	void WorldPhysics::addCapsule(float radius, float height, Transform& transform, RigidbodyProperties properties) {
		CapsuleCollider* coll = new CapsuleCollider(radius, height);
		addRigidbody(coll, transform, properties);
	}

	void WorldPhysics::addMesh(const Model& model, Transform& transform, RigidbodyProperties properties) {
		MeshCollider* coll = new MeshCollider(model);
		addRigidbody(coll, transform, properties);
	}


	void WorldPhysics::addRigidbody(Collider* collider, Transform& transform, RigidbodyProperties properties) {
		Rigidbody* body = new Rigidbody(collider, transform, properties);

		if (properties.isKinematic)
			kinematicRigidbodies.push_back(body);
		else
			rigidbodies.push_back(body);

		world->addRigidBody(&body->getRigidbody());
	}
}