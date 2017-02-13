#ifndef GODRAY_H
#define GODRAY_H

#include <vec3.hpp>
#include "../utility/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

	class DirectionalLight;
	class GaussianBlur;
	class RenderScene;

	//God ray post effect that draws outgoing light shafts into the scene
	class GodRay : public PostProcessingEffect {

	public:
		GodRay(const RenderScene& scene, glm::vec3 lightPosition, unsigned int samples = 20.f);

		//Determine whether to draw whole scene with light rays or light rays only
		void renderRaysOnly(bool only);

	protected:
		virtual void bindValues();

	private:
		bool raysOnly;
		unsigned int samples;
		glm::vec3 lightPosition;
		const RenderScene& scene;
	};


	//Enhancement of the god ray post effect that can smooth out the light shafts and yield better performance
	class GodRaySmooth : public PostProcessingEffect {

	public:
		GodRaySmooth(GodRay& godRay, GaussianBlur& blur, float resolution = 1.f);

		virtual void setScreen(ScreenQuad& screen);

	protected:
		virtual void bindValues();

	private:
		float resolution;
		GodRay& godRay;
		GaussianBlur& blur;
		FrameBuffer rayBuffer;
		FrameBuffer blurBuffer;
		ScreenQuad* rayScreen = nullptr;
	};
}

#endif