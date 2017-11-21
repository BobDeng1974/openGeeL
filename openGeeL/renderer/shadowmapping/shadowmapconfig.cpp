#include "shader/rendershader.h"
#include "shadowmapconfig.h"

namespace geeL {

	ShadowMapConfiguration::ShadowMapConfiguration()
		: type(ShadowMapType::None), shadowBias(0.f), farPlane(0.f), resolution(ShadowmapResolution::Small),
			softShadowScale(0.f), softShadowResolution(0), intensity(0.f) {}

	ShadowMapConfiguration::ShadowMapConfiguration(const ShadowMapConfiguration& other) 
		: type(other.type)
		, shadowBias(other.shadowBias)
		, farPlane(other.farPlane)
		, softShadowScale(other.softShadowScale)
		, intensity(other.intensity)
		, softShadowResolution(other.softShadowResolution)
		, resolution(other.resolution) {}

	ShadowMapConfiguration::ShadowMapConfiguration(float bias, ShadowMapType type, ShadowmapResolution resolution, 
		float softShadowScale, unsigned int softShadowResolution, float farPlane, float intensity)
			: type(type), shadowBias(bias), resolution(resolution), softShadowScale(softShadowScale), 
				softShadowResolution(softShadowResolution), farPlane(farPlane), intensity(intensity) {}

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



	ShadowmapRepository::ShadowmapRepository() {
		simple2D = new RenderShader("renderer/shadowmapping/shadowmapping.vert", "renderer/shaders/empty.frag");
		variance2D = new RenderShader("renderer/shadowmapping/varshadowmapping.vert", "renderer/shadowmapping/varshadowmapping.frag");
		simpleCube = new RenderShader("renderer/shaders/empty.vert", "renderer/shadowmapping/shadowmapping.geom",
			"renderer/shadowmapping/shadowmapping.frag");

	}

	ShadowmapRepository::~ShadowmapRepository() {
		delete simple2D;
		delete variance2D;
		delete simpleCube;
	}

	const RenderShader& ShadowmapRepository::getSimple2DShader() const {
		return *simple2D;
	}

	const RenderShader& ShadowmapRepository::getSimpleCubeShader() const {
		return *simpleCube;
	}

	const RenderShader& ShadowmapRepository::getVariance2DShader() const {
		return *variance2D;
	}

}