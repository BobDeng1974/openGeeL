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

		glm::vec3 getLightPosition() const;
		void setLightPosition(glm::vec3 position);

		unsigned int getSampleCount() const;
		void setSampleCount(unsigned int samples);


	protected:
		virtual void bindValues();

	private:
		unsigned int samples;
		glm::vec3 lightPosition;
		const RenderScene& scene;
	};
}

#endif