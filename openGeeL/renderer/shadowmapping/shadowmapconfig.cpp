#include "shadowmapconfig.h"

namespace geeL {

	ShadowMapConfiguration::ShadowMapConfiguration()
		: type(ShadowMapType::None), shadowBias(0.f), farPlane(0.f), resolution(ShadowmapResolution::Small) {}

	ShadowMapConfiguration::ShadowMapConfiguration(float bias, ShadowMapType type, ShadowmapResolution resolution, float farPlane)
		: type(type), shadowBias(bias), resolution(resolution), farPlane(farPlane) {}


	bool ShadowMapConfiguration::useShadowMap() const {
		return type != ShadowMapType::None;
	}

}