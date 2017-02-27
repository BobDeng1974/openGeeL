#include "../transformation/transform.h"
#include "../lighting/spotlight.h"
#include "volumetriclight.h"

#include "../scene.h"
#include "../cameras/camera.h"
#include <iostream>

using namespace glm;
using namespace std;

namespace geeL {

	VolumetricLight::VolumetricLight(const RenderScene& scene, const SpotLight& light, float density, float minDistance, int samples)
		: PostProcessingEffect("renderer/postprocessing/volumetriclight.frag"), 
			scene(scene), light(light), density(density), minDistance(minDistance), samples(samples) {}


	void VolumetricLight::init(ScreenQuad& screen) {
		PostProcessingEffect::init(screen);

		buffers.push_back(light.getShadowMapID());
	}

	void VolumetricLight::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("gPositionDepth", shader.mapOffset + 1);
		shader.setInteger("shadowMap", shader.mapOffset + 2);

		shader.setInteger("effectOnly", onlyEffect);
		shader.setMat4("inverseView", *inverseView);
		shader.setFloat("minCutoff", minDistance);
		shader.setFloat("density", density);
		shader.setInteger("samples", samples);

		light.deferredBind(scene, shader, "light.");
	}


	void VolumetricLight::addWorldInformation(map<WorldMaps, unsigned int> maps, map<WorldMatrices, const mat4*> matrices,
		map<WorldVectors, const vec3*> vectors) {

		addBuffer( { maps[WorldMaps::PositionDepth] });
		inverseView = matrices[WorldMatrices::InverseView];
	}
}