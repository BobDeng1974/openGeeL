#ifndef SHADOWMAPMANAGER_H
#define SHADOWMAPMANAGER_H

#include <vec3.hpp>
#include "utility/depthreader.h"

namespace geeL {

	enum class ShadowmapResolution;

	class Light;
	class Scene;
	class TextureProvider;


	class ShadowmapAllocator {

	public:
		ShadowmapAllocator(const Scene& scene, TextureProvider& provider, 
			unsigned int allocationSize = 4000);

		void update();

	private:
		unsigned int allocationSize;

		DepthReader depthReader;
		const Scene& scene;
		TextureProvider& provider;

		float computeSizeHeuristic(const Light& light, const glm::vec3& position);
		ShadowmapResolution getResolution(float estimate);

	};

}

#endif
