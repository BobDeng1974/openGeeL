#ifndef VOLUMETRIC_H
#define VOLUMETRIC_H

#include "postprocessing.h"
#include "../utility/worldinformation.h"

namespace geeL {

	class RenderScene;
	class SpotLight;

	class VolumetricLight : public PostProcessingEffect, public WorldInformationRequester {

	public:
		VolumetricLight(const RenderScene& scene, const SpotLight& light, 
			float density = 1.f, float minDistance = 1.f, int samples = 30);

		virtual void init(ScreenQuad& screen);

		virtual void addWorldInformation(std::map<WorldMaps, unsigned int> maps,
			std::map<WorldMatrices, const glm::mat4*> matrices,
			std::map<WorldVectors, const glm::vec3*> vectors) override;

	protected:
		virtual void bindValues();

	private:
		int samples;
		float density;
		float minDistance;
		const RenderScene& scene;
		const SpotLight& light;
		const glm::mat4* inverseView;

	};
}

#endif
