#ifndef GODRAY_H
#define GODRAY_H

#include <vec3.hpp>
#include "framebuffer/framebuffer.h"
#include "utility/worldinformation.h"
#include "postprocessing.h"

namespace geeL {

	class DirectionalLight;
	class RenderScene;


	//God ray post effect that draws outgoing light shafts into the scene
	class GodRay : public PostProcessingEffectFS, public CameraRequester {

	public:
		GodRay(glm::vec3 lightPosition, unsigned int samples = 20.f);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();

		glm::vec3 getLightPosition() const;
		void setLightPosition(glm::vec3 position);

		unsigned int getSampleCount() const;
		void setSampleCount(unsigned int samples);

		virtual std::string toString() const;

	private:
		glm::vec3 lightPosition;

		ShaderLocation lightLocation;
		ShaderLocation lightViewLocation;
	};

	inline std::string GodRay::toString() const {
		return "GodRay";
	}

}

#endif