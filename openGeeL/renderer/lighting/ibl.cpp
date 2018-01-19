#include "cameras/camera.h"
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "framebuffer/framebuffer.h"
#include "lights/lightmanager.h"
#include "materials/materialfactory.h"
#include "appglobals.h"
#include "renderscene.h"
#include "ibl.h"

namespace geeL {

	ImageBasedLighting::ImageBasedLighting(RenderScene& scene)
		: SceneRender(scene)
		, AdditiveEffect("shaders/lighting/deferredlighting.vert",
			"shaders/lighting/ibl.frag") 
		, factory(nullptr) {}


	void ImageBasedLighting::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		factory = parameter.factory;
		activateTransparentIBL(true);

		assert(provider != nullptr);
		addTextureSampler(provider->requestAlbedo(), "gDiffuse");
		addTextureSampler(provider->requestPosition(), "gPosition");
		addTextureSampler(provider->requestNormal(), "gNormal");
		addTextureSampler(provider->requestProperties(), "gProperties");

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

	void ImageBasedLighting::setActive(bool value) {
		AdditiveEffect::setActive(value);

		activateTransparentIBL(value);
	}


	void ImageBasedLighting::bindValues() {
		scene.getLightmanager().bindReflectionProbes(*camera, shader, ShaderTransformSpace::View);
		camera->bindInverseViewMatrix(shader, invViewLocation);

		shader.bind<glm::vec3>(originLocation, camera->GetOriginInViewSpace());
	}


	void ImageBasedLighting::activateTransparentIBL(bool activate) {
		assert(factory != nullptr);

		std::list<SceneShader*> shaders;
		shaders.push_back(&factory->getDefaultShader(ShadingMethod::Transparent, false));
		shaders.push_back(&factory->getDefaultShader(ShadingMethod::Transparent, true));

		for (auto it(shaders.begin()); it != shaders.end(); it++) {
			SceneShader& shader = **it;

			if (active) {
				shader.bind<int>("useIBL", 1);
				scene.getLightmanager().addReflectionProbes(shader);
				scene.getLightmanager().bindReflectionProbes(*camera, shader, shader.getSpace());
			}
			else {
				shader.bind<int>("useIBL", 0);
				scene.getLightmanager().removeReflectionProbes(shader);
			}
		}
	}

}