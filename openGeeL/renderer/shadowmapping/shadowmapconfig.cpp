#include "shadowmapconf.h"

namespace geeL {

	ShadowMapConfiguration::ShadowMapConfiguration()
		: useShadowMap(false), shadowBias(0.f), farPlane(0.f), resolution(ShadowmapResolution::Small) {}

	ShadowMapConfiguration::ShadowMapConfiguration(float bias, ShadowmapResolution resolution, float farPlane)
		: useShadowMap(true), shadowBias(bias), resolution(resolution), farPlane(farPlane) {}


}