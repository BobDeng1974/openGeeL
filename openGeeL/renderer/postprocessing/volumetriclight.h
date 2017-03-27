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
			float density = 1.f, float minDistance = 1.f, unsigned int samples = 30);

		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

		virtual void addWorldInformation(std::map<WorldMaps, unsigned int> maps,
			std::map<WorldMatrices, const glm::mat4*> matrices,
			std::map<WorldVectors, const glm::vec3*> vectors) override;

		unsigned int getSampleCount() const;
		void setSampleCount(unsigned int samples);

		float getDensity() const;
		void setDensity(float density);

		float getMinDistance() const;
		void setMinDistance(float distance);


	protected:
		virtual void bindValues();

	private:
		unsigned int samples;
		float density;
		float minDistance;
		const RenderScene& scene;
		const SpotLight& light;
		const glm::mat4* inverseView;
		const glm::mat4x4* projectionMatrix;

		ShaderLocation projectionLocation;
		ShaderLocation invViewLocation;

	};
}

#endif
