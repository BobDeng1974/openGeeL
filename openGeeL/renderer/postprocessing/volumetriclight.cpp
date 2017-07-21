#include "../transformation/transform.h"
#include "../lights/spotlight.h"
#include "volumetriclight.h"
#include "../scene.h"
#include "../shader/sceneshader.h"
#include "../cameras/camera.h"
#include "../shadowmapping/shadowmap.h"
#include <iostream>

using namespace glm;
using namespace std;

namespace geeL {

	VolumetricLight::VolumetricLight(const SpotLight& light, float density, float minDistance, unsigned int samples, bool useCookie)
		: PostProcessingEffectFS("renderer/postprocessing/volumetriclight.frag"), 
			light(light), density(density), minDistance(minDistance), samples(samples), useCookie(useCookie) {}


	void VolumetricLight::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		const ShadowMap* map = light.getShadowMap();

		if (map != nullptr)
			addImageBuffer(*map, "shadowMap");
		else
			std::cout << "Volumetric light not functional since light has no shadow map attached\n";

		const Texture* lightCookie = light.getLightCookie();
		if (lightCookie != nullptr) {
			addImageBuffer(*lightCookie, "lightCookie");
			shader.bind<int>("useCookie", (int)useCookie);
		}

		shader.bind<int>("effectOnly", onlyEffect);
		shader.bind<float>("minCutoff", minDistance);
		shader.bind<float>("density", density);
		shader.bind<int>("samples", samples);

		projectionLocation = shader.getLocation("projection");
		invViewLocation = shader.getLocation("inverseView");
	}

	void VolumetricLight::bindValues() {
		shader.bind<glm::mat4>(invViewLocation, camera->getInverseViewMatrix());
		shader.bind<glm::mat4>(projectionLocation, camera->getProjectionMatrix());

		light.bind(*camera, shader, "light.", ShaderTransformSpace::View);
	}


	void VolumetricLight::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		addImageBuffer(*maps[WorldMaps::PositionRoughness], "gPositionDepth");
	}

	unsigned int VolumetricLight::getSampleCount() const {
		return samples;
	}

	void VolumetricLight::setSampleCount(unsigned int samples) {
		if (samples < 500 && samples != this->samples) {
			this->samples = samples;

			shader.use();
			shader.bind<int>("samples", samples);
		}
	}

	float VolumetricLight::getDensity() const {
		return density;
	}

	void VolumetricLight::setDensity(float density) {
		if (density > 0.f && density != this->density) {
			this->density = density;

			shader.use();
			shader.bind<float>("density", density);
		}
	}


	float VolumetricLight::getMinDistance() const {
		return minDistance;
	}

	void VolumetricLight::setMinDistance(float distance) {
		if (distance > 0.f && distance != minDistance) {
			minDistance = distance;

			shader.use();
			shader.bind<float>("minCutoff", minDistance);
		}
	}

}