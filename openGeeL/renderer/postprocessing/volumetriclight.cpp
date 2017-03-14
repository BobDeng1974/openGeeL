#include "../transformation/transform.h"
#include "../lighting/spotlight.h"
#include "volumetriclight.h"

#include "../scene.h"
#include "../cameras/camera.h"
#include <iostream>

using namespace glm;
using namespace std;

namespace geeL {

	VolumetricLight::VolumetricLight(const RenderScene& scene, const SpotLight& light, float density, float minDistance, unsigned int samples)
		: PostProcessingEffect("renderer/postprocessing/volumetriclight.frag"), 
			scene(scene), light(light), density(density), minDistance(minDistance), samples(samples) {}


	void VolumetricLight::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		buffers.push_back(light.getShadowMapID());

		unsigned int cookieID = light.getLightCookieID();
		if(cookieID != 0)
			buffers.push_back(cookieID);
	}

	void VolumetricLight::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("gPositionDepth", shader.mapOffset + 1);
		shader.setInteger("shadowMap", shader.mapOffset + 2);
		shader.setInteger("lightCookie", shader.mapOffset + 3);

		shader.setInteger("effectOnly", onlyEffect);
		shader.setMat4("inverseView", *inverseView);
		shader.setMat4("projection", *projectionMatrix);
		shader.setFloat("minCutoff", minDistance);
		shader.setFloat("density", density);
		shader.setInteger("samples", samples);

		light.deferredBind(scene, shader, "light.");
	}


	void VolumetricLight::addWorldInformation(map<WorldMaps, unsigned int> maps, map<WorldMatrices, const mat4*> matrices,
		map<WorldVectors, const vec3*> vectors) {

		addBuffer( { maps[WorldMaps::PositionDepth] });
		inverseView = matrices[WorldMatrices::InverseView];
		projectionMatrix = matrices[WorldMatrices::Projection];
	}

	unsigned int VolumetricLight::getSampleCount() const {
		return samples;
	}

	void VolumetricLight::setSampleCount(unsigned int samples) {
		if (samples < 500)
			this->samples = samples;
	}

	float VolumetricLight::getDensity() const {
		return density;
	}

	void VolumetricLight::setDensity(float density) {
		if (density > 0.f)
			this->density = density;
	}


	float VolumetricLight::getMinDistance() const {
		return minDistance;
	}

	void VolumetricLight::setMinDistance(float distance) {
		if (distance > 0.f)
			minDistance = distance;
	}

}