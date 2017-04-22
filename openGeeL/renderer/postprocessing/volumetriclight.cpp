#include "../transformation/transform.h"
#include "../lighting/spotlight.h"
#include "volumetriclight.h"
#include "../scene.h"
#include "../shader/sceneshader.h"
#include "../cameras/camera.h"
#include "../shadowmapping/shadowmap.h"
#include <iostream>

using namespace glm;
using namespace std;

namespace geeL {

	VolumetricLight::VolumetricLight(const SpotLight& light, float density, float minDistance, unsigned int samples)
		: PostProcessingEffect("renderer/postprocessing/volumetriclight.frag"), 
			light(light), density(density), minDistance(minDistance), samples(samples) {}


	void VolumetricLight::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		const ShadowMap* map = light.getShadowMap();

		if (map != nullptr)
			addBuffer(map->getID(), "shadowMap");
		else
			std::cout << "Volumetric light not functional since light has no shadow map attached\n";

		unsigned int cookieID = light.getLightCookieID();
		if (cookieID != 0)
			addBuffer(cookieID, "lightCookie");

		shader.setInteger("effectOnly", onlyEffect);
		shader.setFloat("minCutoff", minDistance);
		shader.setFloat("density", density);
		shader.setInteger("samples", samples);

		projectionLocation = shader.getLocation("projection");
		invViewLocation = shader.getLocation("inverseView");
	}

	void VolumetricLight::bindValues() {
		shader.setMat4(invViewLocation, camera->getInverseViewMatrix());
		shader.setMat4(projectionLocation, camera->getProjectionMatrix());

		light.bind(*camera, shader, "light.", ShaderTransformSpace::View);
	}


	void VolumetricLight::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		addBuffer(*maps[WorldMaps::PositionDepth], "gPositionDepth");
	}

	unsigned int VolumetricLight::getSampleCount() const {
		return samples;
	}

	void VolumetricLight::setSampleCount(unsigned int samples) {
		if (samples < 500 && samples != this->samples) {
			this->samples = samples;

			shader.use();
			shader.setInteger("samples", samples);
		}
	}

	float VolumetricLight::getDensity() const {
		return density;
	}

	void VolumetricLight::setDensity(float density) {
		if (density > 0.f && density != this->density) {
			this->density = density;

			shader.use();
			shader.setFloat("density", density);
		}
	}


	float VolumetricLight::getMinDistance() const {
		return minDistance;
	}

	void VolumetricLight::setMinDistance(float distance) {
		if (distance > 0.f && distance != minDistance) {
			minDistance = distance;

			shader.use();
			shader.setFloat("minCutoff", minDistance);
		}
	}

}