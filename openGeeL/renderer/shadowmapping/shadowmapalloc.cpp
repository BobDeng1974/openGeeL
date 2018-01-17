#include <map>
#include "cameras/camera.h"
#include "lights/lightmanager.h"
#include "texturing/textureprovider.h"
#include "transformation/transform.h"
#include "renderscene.h"
#include "shadowmap.h"
#include "shadowmapalloc.h"
#include <iostream>

namespace geeL {
	
	ShadowmapAllocator::ShadowmapAllocator(const Scene& scene, TextureProvider& provider, 
		unsigned int allocationSize)
			: provider(provider) 
			, scene(scene)
			, depthReader(provider)
			, allocationSize(allocationSize) {}


	void ShadowmapAllocator::update() {
		depthReader.readDepth();
		float depth = depthReader.getDepth();

		const SceneCamera& camera = scene.getCamera();
		glm::vec3 position = camera.transform.getPosition() + depth * camera.transform.getForwardDirection();

		float total = 0.f;
		std::map<ShadowMap*, float> maps;

		const LightManager& lightManager = scene.getLightmanager();
		lightManager.iterShadowmaps([&total, &maps, &position, this](ShadowMap& map) {
			const Light& light = map.getLight();

			//Filter out static and/or directional light sources
			if (!light.transform.isStatic && light.getLightType() != LightType::Directional) {
				float h = computeSizeHeuristic(light, position);

				maps[&map] = h;
				total += h;
			}
		});

		unsigned int normalizedAllocationSize = allocationSize / total;
		for (auto it(maps.begin()); it != maps.end(); it++) {
			ShadowMap& map = *it->first;
			float h = it->second;

			float sizeEstimate = normalizedAllocationSize * h;
			ShadowmapResolution size = getResolution(sizeEstimate);

			map.resize(size);
		}


		//std::cout << std::to_string(depth) << "\n";
	}


	float ShadowmapAllocator::computeSizeHeuristic(const Light& light, const glm::vec3& position) {
		//TODO: implement this

		return 0.0f;
	}


	ShadowmapResolution ShadowmapAllocator::getResolution(float estimate) {
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
			if (estimate <= (int)ShadowmapResolution::VeryHigh)
				return ShadowmapResolution::VeryHigh;
			else
				ShadowmapResolution::Huge;

		}
	}



}