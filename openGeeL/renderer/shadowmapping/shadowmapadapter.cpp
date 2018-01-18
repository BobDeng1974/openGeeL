#include <algorithm>
#include <iostream>
#include <map>
#include "cameras/camera.h"
#include "lights/lightmanager.h"
#include "texturing/textureprovider.h"
#include "transformation/transform.h"
#include "renderscene.h"
#include "shadowmap.h"
#include "shadowmapadapter.h"

using namespace std;


namespace geeL {
	
	ShadowmapAdapter::ShadowmapAdapter(const Scene& scene, TextureProvider& provider, unsigned int allocationSize) 
		: provider(provider)
		, scene(scene)
		, depthReader(provider)
		, allocationSize(allocationSize)
		, depthScale(50.f) 
		, attenuationScale(0.33f)
		, baseSizeScale(2.f) {}


	void ShadowmapAdapter::update() {
		depthReader.readDepth();
		float depth = depthReader.getDepth();

		const SceneCamera& camera = scene.getCamera();
		float normalizedDepth = depth / camera.getFarPlane();
		glm::vec3 position = camera.transform.getPosition() + depth * camera.transform.getForwardDirection();

		float total = 0.f;
		std::map<ShadowMap*, float> maps;

		const LightManager& lightManager = scene.getLightmanager();
		lightManager.iterShadowmaps([&total, &maps, &position, &normalizedDepth, this](ShadowMap& map) {
			const Light& light = map.getLight();

			//Filter out static and/or directional light sources
			if (!light.transform.isStatic && light.getLightType() != LightType::Directional) {
				float h = computeSizeHeuristic(light, position, normalizedDepth);

				maps[&map] = h;
				total += h;
			}
		});


		unsigned int allocationSpace = allocationSize;
		for (auto it(maps.begin()); it != maps.end(); it++) {
			ShadowMap& map = *it->first;
			float h = it->second;

			float allocationShare = (h / total) * allocationSpace;
			total -= h;

			float sizeEstimate = min(h, allocationShare);
			ShadowmapResolution newSize = getResolution(sizeEstimate);

			//Use targeted shadowmap resolution as upper bound for new resolution
			ShadowmapResolution targetSize = map.getShadowResolution();
			ShadowmapResolution finalSize = (targetSize < newSize) ? targetSize : newSize;

			allocationSpace -= static_cast<unsigned int>(finalSize);
			map.resize(finalSize);

			//std::cout << std::to_string(sizeEstimate) << " : " << std::to_string((int)finalSize) << "\n";
		}
	}

	float ShadowmapAdapter::computeSizeHeuristic(const Light& light, const glm::vec3& position, float depth) {
		float weightedDepth = pow(1.f - depth, depthScale);

		float attenuation = min(light.getAttenuation(position) * light.getIntensity(), 1.f);
		attenuation = pow(attenuation, attenuationScale);

		return (baseSizeScale * allocationSize) * weightedDepth * attenuation;
	}


	ShadowmapResolution ShadowmapAdapter::getResolution(float estimate) {
		//Lazy binary search to find closest resolution

		if (estimate <= (int)ShadowmapResolution::High) {
			if (estimate <= (int)ShadowmapResolution::Small) {
				if (estimate <= (int)ShadowmapResolution::Tiny)
					return ShadowmapResolution::Tiny;
				else
					return ShadowmapResolution::Small;
			}
			else {
				if (estimate <= (int)ShadowmapResolution::Medium)
					return ShadowmapResolution::Medium;
				else
					return ShadowmapResolution::High;
			}
		}
		else {
			if (estimate <= (int)ShadowmapResolution::Large) {
				if (estimate <= (int)ShadowmapResolution::VeryHigh)
					return ShadowmapResolution::VeryHigh;
				else
					return ShadowmapResolution::Large;
			}
			else
				return ShadowmapResolution::Huge;
		}
	}



	float ShadowmapAdapter::getDepthScale() const {
		return depthScale;
	}

	float ShadowmapAdapter::getAttenuationScale() const {
		return attenuationScale;
	}

	float ShadowmapAdapter::getBaseSizeScale() const {
		return baseSizeScale;
	}

	void ShadowmapAdapter::setDepthScale(float value) {
		if (value > 0.f)
			depthScale = value;

	}

	void ShadowmapAdapter::setAttenuationScale(float value) {
		if (value > 0.f)
			attenuationScale = value;

	}

	void ShadowmapAdapter::setBaseSizeScale(float value) {
		if (value > 0.f)
			baseSizeScale = value;
	}

}