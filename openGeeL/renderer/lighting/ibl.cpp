#include "cameras/camera.h"
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "lights/lightmanager.h"
#include "renderscene.h"
#include "ibl.h"

namespace geeL {

	ImageBasedLighting::ImageBasedLighting(RenderScene& scene)
		: SceneRender(scene)
		, AdditiveEffect("renderer/lighting/deferredlighting.vert",
			"renderer/lighting/ibl.frag") {}


	void ImageBasedLighting::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		assert(provider != nullptr);
		addTextureSampler(provider->requestAlbedo(), "gDiffuse");
		addTextureSampler(provider->requestPositionRoughness(), "gPositionRoughness");
		addTextureSampler(provider->requestNormalMetallic(), "gNormalMet");

		const Texture* occlusion = provider->requestOcclusion();
		if (occlusion != nullptr) {
			addTextureSampler(*occlusion, "ssao");
			shader.bind<int>("useSSAO", 1);
		}

		scene.getLightmanager().addReflectionProbes(shader);
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void ImageBasedLighting::draw() {
		PostProcessingEffectFS::draw();
	}

	void ImageBasedLighting::bindValues() {
		scene.getLightmanager().bindReflectionProbes(*camera, shader, ShaderTransformSpace::View);
		camera->bindInverseViewMatrix(shader, invViewLocation);

		shader.bind<glm::vec3>(originLocation, camera->GetOriginInViewSpace());
	}

}