#include <thread>
#include <chrono>
#include <btBulletDynamicsCommon.h>
#include "application.h"
#include "collider.h"
#include "rigidbody.h"
#include "worldphysics.h"

using namespace std;

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
	}

	void WorldPhysics::run() {
		while (!Application::closing()) {
			update();
			this_thread::sleep_for(chrono::milliseconds(33));
		}
	}

	void WorldPhysics::intersect(glm::vec3 start, glm::vec3 end, RayCastHit& hit) const {
		btVector3 s = btVector3(start.x, start.y, start.z);
		btVector3 e = btVector3(end.x, end.y, end.z);

		btCollisionWorld::ClosestRayResultCallback callback(s, e);
		world->rayTest(s, e, callback);

		if (callback.hasHit()) {
			btVector3 p = callback.m_hitPointWorld;
			btVector3 n = callback.m_hitNormalWorld;

			hit.point = glm::vec3(p.getX(), p.getY(), p.getZ());
			hit.normal = glm::vec3(n.getX(), n.getY(), n.getZ());
			hit.hit = true;
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

	void WorldPhysics::addStaticMesh(const Model& model, Transform& transform, RigidbodyProperties properties) {
		StaticMeshCollider* coll = new StaticMeshCollider(model);
		addRigidbody(coll, transform, properties);
	}


	void WorldPhysics::addRigidbody(Collider* collider, Transform& transform, RigidbodyProperties properties) {
		Rigidbody* body = new Rigidbody(collider, transform, properties);

		rigidbodies.push_back(body);
		world->addRigidBody(&body->getRigidbody());
	}
}