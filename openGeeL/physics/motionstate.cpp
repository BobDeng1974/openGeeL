#include "transformation/transform.h"
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

		transform.setEulerAngles(glm::vec3(yaw, pitch, roll));
	}

	void MotionState::update() {
		btQuaternion rotation = btQuaternion(transform.getRotation().x,
			transform.getRotation().y,
			transform.getRotation().z);

		btVector3 position = btVector3(transform.getPosition().x,
			transform.getPosition().y,
			transform.getPosition().z);

		initTransform = btTransform(rotation, position);
	}


	KinematicMotionState::KinematicMotionState(Transform& transform) 
		: MotionState(transform) {
	
		transform.addListener(*this);
	}

	KinematicMotionState::~KinematicMotionState() {
		transform.removeListener(*this);
	}


	void KinematicMotionState::setWorldTransform(const btTransform& trans) {}

	void KinematicMotionState::onChange(const Transform & t) {
		update();
	}

}