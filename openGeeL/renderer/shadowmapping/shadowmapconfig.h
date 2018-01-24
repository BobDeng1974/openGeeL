#ifndef SHADOWMAPINFO_H
#define SHADOWMAPINFO_H
 
namespace geeL {

	class RenderShader;


	enum class ShadowMapType {
		None = 0,
		Hard = 1,
		Soft = 2
	};


	enum class ShadowmapResolution {
		Tiny = 128,
		Small = 256,
		Medium = 512,
		High = 768,
		VeryHigh = 1024,
		Large = 2048,
		Huge = 4096
	};


	//Container class for shadow map configurations
	struct ShadowMapConfiguration {

	public:
		ShadowMapType type;
		float shadowBias;
		float farPlane;
		float softShadowScale;
		float intensity;
		unsigned int softShadowResolution;
		ShadowmapResolution resolution;

		ShadowMapConfiguration();
		ShadowMapConfiguration(const ShadowMapConfiguration& other);
		ShadowMapConfiguration(float bias, ShadowMapType type, ShadowmapResolution resolution = ShadowmapResolution::High,
			float softShadowScale = 1.f, unsigned int softShadowResolution = 8, float farPlane = 100, float intensity = 1.f);

		ShadowMapConfiguration& operator=(const ShadowMapConfiguration& other);

		bool useShadowMap() const;

	};


	static ShadowMapConfiguration noShadowMapConfig    = ShadowMapConfiguration();
	static ShadowMapConfiguration defPLShadowMapConfig = ShadowMapConfiguration(0.0001f, ShadowMapType::Soft, 
		ShadowmapResolution::High, 7.f, 2);
	static ShadowMapConfiguration defSLShadowMapConfig = ShadowMapConfiguration(0.0001f, ShadowMapType::Soft);
	static ShadowMapConfiguration defDLShadowMapConfig = ShadowMapConfiguration(0.00002f, ShadowMapType::Soft, 
		ShadowmapResolution::VeryHigh, 1.f, 1);



	//Repository of default shadow map shaders
	class ShadowmapRepository {

	public:
		ShadowmapRepository();
		~ShadowmapRepository();

		const RenderShader& getSimple2DShader();
		const RenderShader& getSimpleCubeShader();
		const RenderShader& getVariance2DShader();

		const RenderShader& getSimple2DAnimated();
		const RenderShader& getSimpleCubeAnimated();

	private:
		RenderShader* simple2D;
		RenderShader* variance2D;
		RenderShader* simpleCube;
		RenderShader* simple2DAnim;
		RenderShader* simpleCubeAnim;

	};



	inline bool operator<(ShadowmapResolution a, ShadowmapResolution b) {
		return static_cast<int>(a) < static_cast<int>(b);
	}

	inline ShadowmapResolution getShadowResolution(float estimate) {
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

}

#endif
