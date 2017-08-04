#ifndef MOVABLECAMERA_H
#define MOVABLECAMERA_H

#include "scripting/component.h"
#include "camera.h"

namespace geeL {

	class MovableCamera : public GenericComponent<Camera> {

	public:
		MovableCamera(float speed, float sensitivity);

		virtual void update(Input& input);


		float getSpeed() const;
		float getSensitivity() const;

		void setSpeed(float speed);
		void setSensitivity(float sensitivity);

	private:
		float speed, sensitivity;

		virtual void computeKeyboardInput(const Input& input);
		virtual void computeMouseInput(const Input& input);

	};

}

#endif
