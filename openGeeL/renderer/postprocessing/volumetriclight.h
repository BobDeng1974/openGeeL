#ifndef VOLUMETRIC_H
#define VOLUMETRIC_H

#include "worldpostprocessing.h"

namespace geeL {

	class RenderScene;
	class SpotLight;

	class VolumetricLight : public WorldPostProcessingEffect {

	public:
		VolumetricLight(const RenderScene& scene, const SpotLight& light, 
			float density = 1.f, float minDistance = 1.f, int samples = 30);

		virtual void init(ScreenQuad& screen);

		virtual WorldMaps requiredWorldMaps() const;
		virtual WorldMatrices requiredWorldMatrices() const;
		virtual WorldVectors requiredWorldVectors() const;

		virtual std::list<WorldMaps> requiredWorldMapsList() const;
		virtual std::list<WorldMatrices> requiredWorldMatricesList() const;

		virtual void addWorldInformation(std::list<unsigned int> maps,
			std::list<const glm::mat4*> matrices, std::list<const glm::vec3*> vectors);

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
