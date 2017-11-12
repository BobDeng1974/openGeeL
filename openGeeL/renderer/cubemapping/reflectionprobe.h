#ifndef REFLECTIONPROBE_H
#define REFLECTIONPROBE_H

#include <functional>
#include <string>
#include "sceneobject.h"
#include "cubemap.h"

namespace geeL {

	class Camera;
	class CubeBuffer;
	class FrameBuffer;
	class Transform;

	class ReflectionProbe : public DynamicCubeMap, public SceneObject {

	public:
		ReflectionProbe(CubeBuffer& frameBuffer, 
			std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall,
			Transform& transform, 
			unsigned int resolution, 
			float width = 50.f, 
			float height = 50.f, 
			float depth = 50.f, 
			std::string name = "ReflectionProbe");

		virtual ~ReflectionProbe();

		virtual void bind(const Camera& camera, const RenderShader& shader,
			const std::string& name, ShaderTransformSpace space) const;

		virtual void draw();

	private:
		CubeBuffer& frameBuffer;
		float width, height, depth;

		std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall;

	};

}

#endif
