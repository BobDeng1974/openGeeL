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
	static ShadowMapConfiguration defPLShadowMapConfig = ShadowMapConfiguration(0.0001f, ShadowMapType::Soft, ShadowmapResolution::High, 4.f, 16);
	static ShadowMapConfiguration defSLShadowMapConfig = ShadowMapConfiguration(0.0001f, ShadowMapType::Soft);
	static ShadowMapConfiguration defDLShadowMapConfig = ShadowMapConfiguration(0.00002f, ShadowMapType::Soft);



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

}

#endif
