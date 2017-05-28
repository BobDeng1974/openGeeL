#ifndef SHADOWMAPINFO_H
#define SHADOWMAPINFO_H
 
#include "simpleshadowmap.h"

namespace geeL {

	//Container class for shadow map configurations
	struct ShadowMapConfiguration {

	public:
		const bool useShadowMap;
		const float shadowBias;
		const float farPlane;
		const ShadowmapResolution resolution;

		ShadowMapConfiguration();
		ShadowMapConfiguration(float bias, ShadowmapResolution resolution = ShadowmapResolution::Adaptive, float farPlane = 100);

	};


	static ShadowMapConfiguration noShadowMapConfig    = ShadowMapConfiguration();
	static ShadowMapConfiguration defPLShadowMapConfig = ShadowMapConfiguration(0.0001f);
	static ShadowMapConfiguration defSLShadowMapConfig = ShadowMapConfiguration(0.0001f);
	static ShadowMapConfiguration defDLShadowMapConfig = ShadowMapConfiguration(0.00002f);

}

#endif
