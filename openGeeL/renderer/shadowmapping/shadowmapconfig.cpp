#include "shader/rendershader.h"
#include "shadowmapconfig.h"

namespace geeL {

	ShadowMapConfiguration::ShadowMapConfiguration()
		: type(ShadowMapType::None)
		, shadowBias(0.f)
		, farPlane(0.f)
		, resolution(ShadowmapResolution::Small)
		, softShadowScale(0.f)
		, softShadowResolution(0)
		, intensity(0.f) {}

	ShadowMapConfiguration::ShadowMapConfiguration(const ShadowMapConfiguration& other) 
		: type(other.type)
		, shadowBias(other.shadowBias)
		, farPlane(other.farPlane)
		, softShadowScale(other.softShadowScale)
		, intensity(other.intensity)
		, softShadowResolution(other.softShadowResolution)
		, resolution(other.resolution) {}

	ShadowMapConfiguration::ShadowMapConfiguration(float bias,
		ShadowMapType type, 
		ShadowmapResolution resolution, 
		float softShadowScale, 
		unsigned int softShadowResolution, 
		float farPlane, 
		float intensity)
			: type(type)
			, shadowBias(bias)
			, resolution(resolution)
			, softShadowScale(softShadowScale)
			, softShadowResolution(softShadowResolution)
			, farPlane(farPlane)
			, intensity(intensity) {}

	ShadowMapConfiguration& ShadowMapConfiguration::operator=(const ShadowMapConfiguration& other) {
		if (this != &other) {
			type = other.type;
			shadowBias = other.shadowBias;
			farPlane = other.farPlane;
			softShadowScale = other.softShadowScale;
			intensity = other.intensity;
			softShadowResolution = other.softShadowResolution;
			resolution = other.resolution;
		}

		return *this;
	}


	bool ShadowMapConfiguration::useShadowMap() const {
		return type != ShadowMapType::None;
	}



	ShadowmapRepository::ShadowmapRepository()
		: simple2D(nullptr)
		, simpleCube(nullptr)
		, variance2D(nullptr)
		, simple2DAnim(nullptr)
		, simpleCubeAnim(nullptr) {}

	ShadowmapRepository::~ShadowmapRepository() {
		if(simple2D != nullptr) delete simple2D;
		if (simpleCube != nullptr) delete simpleCube;
		if (variance2D != nullptr) delete variance2D;
		if (simple2DAnim != nullptr) delete simple2DAnim;
		if (simpleCubeAnim != nullptr) delete simpleCubeAnim;
	}

	const RenderShader& ShadowmapRepository::getSimple2DShader() {
		if (simple2D == nullptr) {
			simple2D = new RenderShader("shaders/shadowmapping/shadowmapping.vert", 
				"shaders/empty.frag");
		}

		return *simple2D;
	}

	const RenderShader& ShadowmapRepository::getSimpleCubeShader() {
		if (simpleCube == nullptr) {
			simpleCube = new RenderShader("shaders/empty.vert",
				"shaders/shadowmapping/shadowmapping.frag",
				"shaders/shadowmapping/shadowmapping.geom");
		}

		return *simpleCube;
	}

	const RenderShader& ShadowmapRepository::getVariance2DShader() {
		if (variance2D == nullptr) {
			variance2D = new RenderShader("shaders/shadowmapping/varshadowmapping.vert", 
				"shaders/shadowmapping/varshadowmapping.frag");
		}

		return *variance2D;
	}

	const RenderShader& ShadowmapRepository::getSimple2DAnimated() {
		if (simple2DAnim == nullptr) {
			simple2DAnim = new RenderShader("shaders/shadowmapping/shadowmappingAnim.vert",
				"shaders/empty.frag");
		}

		return *simple2DAnim;
	}

	const RenderShader& ShadowmapRepository::getSimpleCubeAnimated() {
		if (simpleCubeAnim == nullptr) {
			simpleCubeAnim = new RenderShader("shaders/emptyAnim.vert",
				"shaders/shadowmapping/shadowmapping.frag",
				"shaders/shadowmapping/shadowmapping.geom");
		}

		return *simpleCubeAnim;
	}

}