#include "cameras/camera.h"
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "lights/lightmanager.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "renderscene.h"
#include "deferredlighting.h"

namespace geeL {

	DeferredLighting::DeferredLighting(RenderScene& scene) 
		: SceneRender(scene)
		, PostProcessingEffectFS( "renderer/lighting/deferredlighting.vert",
			"renderer/lighting/deferredlighting.frag") {}


	void DeferredLighting::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);
		
		scene.init();

		assert(provider != nullptr);
		addTextureSampler(provider->requestAlbedo(), "gDiffuse");
		addTextureSampler(provider->requestPositionRoughness(), "gPositionRoughness");
		addTextureSampler(provider->requestNormalMetallic(), "gNormalMet");

		const Texture* emissivity = provider->requestEmissivity();
		if (emissivity != nullptr) {
			addTextureSampler(*emissivity, "gEmissivity");
			shader.bind<int>("useEmissivity", 1);
		}

		LightManager& manager = scene.getLightmanager();
		manager.addShaderListener(shader);

		projectionLocation = shader.getLocation("projection");
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void DeferredLighting::draw() {
		PostProcessingEffectFS::draw();
	}

	void DeferredLighting::bindValues() {
		scene.getLightmanager().bind(shader, ShaderTransformSpace::View, camera);

		camera->bindProjectionMatrix(shader, projectionLocation);
		camera->bindInverseViewMatrix(shader, invViewLocation);

		shader.bind<glm::vec3>(originLocation, camera->GetOriginInViewSpace());
	}



	TiledDeferredLighting::TiledDeferredLighting(RenderScene& scene)
		: SceneRender(scene), PostProcessingEffectCS("renderer/lighting/tileddeferred.com.glsl", Resolution(16, 16)) {}


	void TiledDeferredLighting::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectCS::init(parameter);

		scene.init();

		LightManager& manager = scene.getLightmanager();
		manager.addShaderListener(shader);

		projectionLocation = shader.getLocation("projection");
		invViewLocation = shader.getLocation("inverseView");
		originLocation = shader.getLocation("origin");
	}

	void TiledDeferredLighting::draw() {
		PostProcessingEffectCS::draw();
	}

	void TiledDeferredLighting::bindValues() {
		scene.getLightmanager().bind(shader, ShaderTransformSpace::View, camera);

		camera->bindProjectionMatrix(shader, projectionLocation);
		camera->bindInverseViewMatrix(shader, invViewLocation);

		shader.bind<glm::vec3>(originLocation, camera->GetOriginInViewSpace());
	}


}