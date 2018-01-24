#ifndef SHADOWMAPMANAGER_H
#define SHADOWMAPMANAGER_H

#include <vec3.hpp>
#include "utility/depthreader.h"

namespace geeL {

	enum class ShadowmapResolution;

	class Light;
	class Scene;
	class TextureProvider;


	class ShadowmapAdapter {

	public:
		ShadowmapAdapter(const Scene& scene, TextureProvider& provider, 
			unsigned int allocationSize = 4000);

		void update();

		float getDepthScale() const;
		float getAttenuationScale() const;
		float getBaseSizeScale() const;

		void setDepthScale(float value);
		void setAttenuationScale(float value);
		void setBaseSizeScale(float value);

		bool isActive() const;
		void setActive(bool active);

	private:
		bool active;
		unsigned int allocationSize;
		float depthScale, attenuationScale, baseSizeScale;

		DepthReader depthReader;
		const Scene& scene;
		TextureProvider& provider;

		float computeSizeHeuristic(const Light& light, const glm::vec3& position, float depth);

	};

}

#endif
