#include "cameras/camera.h"
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "framebuffer/framebuffer.h"
#include "lights/lightmanager.h"
#include "appglobals.h"
#include "renderscene.h"
#include "ibl.h"

namespace geeL {

	ImageBasedLighting::ImageBasedLighting(RenderScene& scene)
		: SceneRender(scene)
		, AdditiveEffect("shaders/lighting/deferredlighting.vert",
			"shaders/lighting/ibl.frag") {}


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

#if DIFFUSE_SPECULAR_SEPARATION
		assert(parameter.separatedBuffer != nullptr);
		setParent(*parameter.separatedBuffer);
#endif

	}

	void ImageBasedLighting::draw() {
		AdditiveEffect::draw();
	}

	void ImageBasedLighting::fill() {
#if DIFFUSE_SPECULAR_SEPARATION
		BlendGuard blend;
		blend.blendAdd();

		if (parentBuffer != nullptr) {
			RenderTexture& diffuse  = provider->requestCurrentImage();
			RenderTexture& specular = provider->requestCurrentSpecular();
			LayeredTarget combinedTarget(diffuse, specular);

			parentBuffer->add(combinedTarget);
			parentBuffer->fill(*this, clearNothing);
		}
#else
		AdditiveEffect::fill();
#endif 
	}


	void ImageBasedLighting::bindValues() {
		scene.getLightmanager().bindReflectionProbes(*camera, shader, ShaderTransformSpace::View);
		camera->bindInverseViewMatrix(shader, invViewLocation);

		shader.bind<glm::vec3>(originLocation, camera->GetOriginInViewSpace());
	}

}