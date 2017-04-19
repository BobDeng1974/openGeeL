#ifndef REFLECTIONPROBE_H
#define REFLECTIONPROBE_H

#include <functional>
#include <string>
#include "../sceneobject.h"
#include "cubemap.h"

namespace geeL {

	class Camera;
	class Transform;

	class ReflectionProbe : public CubeMap, public SceneObject {

	public:
		ReflectionProbe(std::function<void(const Camera&)> renderCall, Transform& transform, float depth, 
			unsigned int resolution, std::string name = "ReflectionProbe");

		~ReflectionProbe();

		void update();

	private:
		float depth;
		unsigned int resolution, fbo;

		std::function<void(const Camera&)> renderCall;

	};

}

#endif
