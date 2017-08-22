#ifndef WORLDPHYSICS_H
#define WORLDPHYSICS_H

#include <list>
#include "physics.h"
#include "threading.h"

class btDiscreteDynamicsWorld;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;

typedef btDiscreteDynamicsWorld PhysicsWorld;

namespace geeL {

	class Collider;

	//Physics that emulate those of the real world
	class WorldPhysics : public Physics, public ThreadedObject {

	public:
		WorldPhysics();
		virtual ~WorldPhysics();

		virtual void update();

		//Run complete physics simulation.  
		//Should be called in separate thread
		virtual void run();

		virtual void intersect(glm::vec3 start, glm::vec3 end, RayCastHit& hit) const;

		virtual void addSphere(float radius, Transform& transform, RigidbodyProperties properties);
		virtual void addPlane(const glm::vec3 normal, Transform& transform, RigidbodyProperties properties);
		virtual void addBox(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties);
		virtual void addCylinder(const glm::vec3 magnitude, Transform& transform, RigidbodyProperties properties);
		virtual void addCapsule(float radius, float height, Transform& transform, RigidbodyProperties properties);
		virtual void addMesh(const Model& model, Transform& transform, RigidbodyProperties properties);
		virtual void addStaticMesh(const Model& model, Transform& transform, RigidbodyProperties properties);

	private:
		std::list<Rigidbody*> rigidbodies;

		btBroadphaseInterface* broadPhase;
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btSequentialImpulseConstraintSolver* solver;

		PhysicsWorld* world;

		WorldPhysics(const WorldPhysics& other) = delete;
		WorldPhysics& operator= (const WorldPhysics& other) = delete;

		void init();
		void addRigidbody(Collider* collider, Transform& transform, RigidbodyProperties properties);

	};
}

#endif