#ifndef MOTIONSTATE_H
#define MOTIONSTATE_H

#include <btBulletDynamicsCommon.h>
#include "utility/listener.h"

namespace geeL {

	class Transform;

	//Class that forwards transform information from physic to graphic engine
	class MotionState : public btMotionState {

	public:
		MotionState(Transform& transform);
		virtual ~MotionState() {}

		virtual void getWorldTransform(btTransform& trans) const;
		virtual void setWorldTransform(const btTransform& trans);

		void update();

	protected:
		Transform& transform;
		btTransform initTransform;

	};


	//Kinematic motion state that won't be affected 
	//by physics and can be manipulated during runtime
	class KinematicMotionState : public MotionState, public ChangeListener<Transform> {

	public:
		KinematicMotionState(Transform& transform);
		virtual ~KinematicMotionState();

		virtual void setWorldTransform(const btTransform& trans);
		virtual void onChange(const Transform& t);

	};
}

#endif

