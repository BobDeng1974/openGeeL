#include <btBulletDynamicsCommon.h>
#include "../meshes/model.h"
#include "../meshes/mesh.h"
#include "collider.h"

namespace geeL {

	Collider::Collider() : shape(nullptr) {}

	Collider::~Collider() {
		if (shape != nullptr)
			delete shape;
	}


	btCollisionShape* Collider::getShape() const {
		return shape;
	}


	SphereCollider::SphereCollider(float radius) {
		shape = new btSphereShape(radius);
	}


	PlaneCollider::PlaneCollider(const glm::vec3& normal) {
		shape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), 1);
	}


	BoxCollider::BoxCollider(const glm::vec3& magnitude) {
		btVector3 mag = btVector3(magnitude.x, magnitude.y, magnitude.z);
		shape = new btBoxShape(mag);

	}


	CapsuleCollider::CapsuleCollider(float radius, float height) {
		shape = new btCapsuleShape(radius, height);
	}


	CylinderCollider::CylinderCollider(const glm::vec3& magnitude) {
		btVector3 mag = btVector3(magnitude.x, magnitude.y, magnitude.z);
		shape = new btCylinderShape(mag);
	}


	MeshCollider::MeshCollider(const Model& model) {

		btConvexHullShape* temp = new btConvexHullShape();
		for (auto it = model.meshesBeginConst(); it != model.meshesEndConst(); it++) {
			const Mesh& mesh = *it;

			for (unsigned int i = 0; i < mesh.getVerticesCount(); i++) {
				glm::vec3 p1 = mesh.getVertex(i).position;
				temp->addPoint(btVector3(p1.x, p1.y, p1.z));
			}
		}

		shape = temp;
	}


	StaticMeshCollider::StaticMeshCollider(const Model& model) {

		btTriangleMesh tri = btTriangleMesh();
		for (auto it = model.meshesBeginConst(); it != model.meshesEndConst(); it++) {
		const Mesh& mesh = *it;

			for (unsigned int i = 0; i < mesh.getIndicesCount(); i += 3) {
				unsigned int i1 = mesh.getIndex(i);
				unsigned int i2 = mesh.getIndex(i + 1);
				unsigned int i3 = mesh.getIndex(i + 2);

				glm::vec3 p1 = mesh.getVertex(i1).position;
				glm::vec3 p2 = mesh.getVertex(i2).position;
				glm::vec3 p3 = mesh.getVertex(i3).position;

				tri.addTriangle(btVector3(p1.x, p1.y, p1.z), btVector3(p2.x, p2.y, p2.z), btVector3(p3.x, p3.y, p3.z));
			}
		}

		shape = new btConvexTriangleMeshShape(&tri);
	}

}