#include <iostream>
#include "transformation/transform.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "lights/spotlight.h"
#include "volumetriclight.h"
#include "renderscene.h"
#include "shader/sceneshader.h"
#include "cameras/camera.h"
#include "shadowmapping/shadowmap.h"

using namespace glm;
using namespace std;

namespace geeL {

	VolumetricLight::VolumetricLight(SpotLight& light, 
		float density, 
		float minDistance, 
		unsigned int samples, 
		bool useCookie)
			: AdditiveEffect("shaders/postprocessing/volumetriclight.frag")
			, light(light)
			, density(density)
			, minDistance(minDistance)
			, samples(samples)
			, useCookie(useCookie) {}


	void VolumetricLight::init(const PostProcessingParameter& parameter) {
		AdditiveEffect::init(parameter);

		assert(provider != nullptr);
		addTextureSampler(provider->requestPosition(), "gPosition");

		const ShadowMap* map = light.getShadowMap();
		if (map != nullptr)
			addTextureSampler(*map, "shadowMap");
		else
			std::cout << "Volumetric light not functional since light has no shadow map attached\n";

		const ITexture* lightCookie = light.getLightCookie();
		if (lightCookie != nullptr) {
			addTextureSampler(*lightCookie, "lightCookie");
			shader.bind<int>("useCookie", (int)useCookie);
		}

		shader.bind<int>("samples", samples);
		shader.bind<int>("effectOnly", onlyEffect);
		shader.bind<float>("minCutoff", minDistance);
		shader.bind<float>("density", density);
		
		projectionLocation = shader.getLocation("projection");
		invViewLocation = shader.getLocation("inverseView");
	}

	void VolumetricLight::bindValues() {
		camera->bindProjectionMatrix(shader, projectionLocation);
		camera->bindInverseViewMatrix(shader, invViewLocation);

		light.bind(shader, "light.", ShaderTransformSpace::View, camera);
		shader.bind<int>("lightActive", light.isActive());
	}


	unsigned int VolumetricLight::getSampleCount() const {
		return samples;
	}

	void VolumetricLight::setSampleCount(unsigned int samples) {
		if (samples < 500 && samples != this->samples) {
			this->samples = samples;

			shader.bind<int>("samples", samples);
		}
	}

	float VolumetricLight::getDensity() const {
		return density;
	}

	void VolumetricLight::setDensity(float density) {
		if (density > 0.f && density != this->density) {
			this->density = density;

			shader.bind<float>("density", density);
		}
	}


	float VolumetricLight::getMinDistance() const {
		return minDistance;
	}

	void VolumetricLight::setMinDistance(float distance) {
		if (distance > 0.f && distance != minDistance) {
			minDistance = distance;

			shader.bind<float>("minCutoff", minDistance);
		}
	}

}