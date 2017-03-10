#include "../transformation/transform.h"
#include "motionstate.h"

namespace geeL {

	MotionState::MotionState(Transform& transform) : transform(transform) {
		update();
	}


	void MotionState::getWorldTransform(btTransform &worldTrans) const {
		worldTrans = initTransform;
	}

	void MotionState::setWorldTransform(const btTransform &worldTrans) {
		btVector3 p = worldTrans.getOrigin();
		btQuaternion r = worldTrans.getRotation();

		transform.setPosition(glm::vec3(p.getX(), p.getY(), p.getZ()));

		float yaw, pitch, roll;
		r.getEulerZYX(yaw, pitch, roll);

		transform.setRotation(glm::vec3(yaw, pitch, roll));
	}

	void MotionState::update() {
		btQuaternion rotation = btQuaternion(transform.rotation.x,
			transform.rotation.y,
			transform.rotation.z);

		btVector3 position = btVector3(transform.position.x,
			transform.position.y,
			transform.position.z);

		initTransform = btTransform(rotation, position);
	}


	KinematicMotionState::KinematicMotionState(Transform& transform) 
		: MotionState(transform) {
	
		auto callback = [this](const Transform& transform) { this->update(); };
		transform.addChangeListener(callback);
	}


	void KinematicMotionState::setWorldTransform(const btTransform& trans) {}

}