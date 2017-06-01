#ifndef SHADOWMAPINFO_H
#define SHADOWMAPINFO_H
 
namespace geeL {

	enum class ShadowMapType {
		None = 0,
		Hard = 1,
		Soft = 2
	};

	enum class ShadowmapResolution {
		Adaptive = 0,
		Small = 128,
		Medium = 256,
		High = 512,
		VeryHigh = 1024,
		Huge = 4096
	};


	//Container class for shadow map configurations
	struct ShadowMapConfiguration {

	public:
		const ShadowMapType type;
		const float shadowBias;
		const float farPlane;
		const float softShadowScale;
		const unsigned int softShadowResolution;
		const ShadowmapResolution resolution;

		ShadowMapConfiguration();
		ShadowMapConfiguration(float bias, ShadowMapType type, ShadowmapResolution resolution = ShadowmapResolution::Adaptive,
			float softShadowScale = 1.f, unsigned int softShadowResolution = 8, float farPlane = 100);

		bool useShadowMap() const;

	};


	static ShadowMapConfiguration noShadowMapConfig    = ShadowMapConfiguration();
	static ShadowMapConfiguration defPLShadowMapConfig = ShadowMapConfiguration(0.0001f, ShadowMapType::Soft, ShadowmapResolution::Adaptive, 4.f, 16);
	static ShadowMapConfiguration defSLShadowMapConfig = ShadowMapConfiguration(0.0001f, ShadowMapType::Soft);
	static ShadowMapConfiguration defDLShadowMapConfig = ShadowMapConfiguration(0.00002f, ShadowMapType::Soft);

}

#endif
