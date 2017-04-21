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
		ReflectionProbe(std::function<void(const Camera&, FrameBufferInformation)> renderCall, Transform& transform, unsigned int resolution, 
			float width = 50.f, float height = 50.f, float depth = 50.f, std::string name = "ReflectionProbe");

		~ReflectionProbe();

		void update();

	private:
		float width, height, depth;
		unsigned int resolution, fbo;

		std::function<void(const Camera&, FrameBufferInformation)> renderCall;

	};

}

#endif
