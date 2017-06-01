#include "shadowmapconfig.h"

namespace geeL {

	ShadowMapConfiguration::ShadowMapConfiguration()
		: type(ShadowMapType::None), shadowBias(0.f), farPlane(0.f), resolution(ShadowmapResolution::Small),
			softShadowScale(0.f), softShadowResolution(0) {}

	ShadowMapConfiguration::ShadowMapConfiguration(float bias, ShadowMapType type, ShadowmapResolution resolution, 
		float softShadowScale, unsigned int softShadowResolution, float farPlane)
			: type(type), shadowBias(bias), resolution(resolution), softShadowScale(softShadowScale), 
				softShadowResolution(softShadowResolution), farPlane(farPlane) {}


	bool ShadowMapConfiguration::useShadowMap() const {
		return type != ShadowMapType::None;
	}

}