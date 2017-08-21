#include "shadowmapconfig.h"

namespace geeL {

	ShadowMapConfiguration::ShadowMapConfiguration()
		: type(ShadowMapType::None), shadowBias(0.f), farPlane(0.f), resolution(ShadowmapResolution::Small),
			softShadowScale(0.f), softShadowResolution(0), intensity(0.f) {}

	ShadowMapConfiguration::ShadowMapConfiguration(float bias, ShadowMapType type, ShadowmapResolution resolution, 
		float softShadowScale, unsigned int softShadowResolution, float farPlane, float intensity)
			: type(type), shadowBias(bias), resolution(resolution), softShadowScale(softShadowScale), 
				softShadowResolution(softShadowResolution), farPlane(farPlane), intensity(intensity) {}


	bool ShadowMapConfiguration::useShadowMap() const {
		return type != ShadowMapType::None;
	}

}