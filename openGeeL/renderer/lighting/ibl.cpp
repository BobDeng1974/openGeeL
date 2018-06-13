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

	void iterIBLShaders(MaterialFactory& factory, std::function<void(SceneShader&)> function) {
		SceneShader& a = factory.getDefaultShader(ShadingMethod::Transparent, false);
		SceneShader& b = factory.getDefaultShader(ShadingMethod::Transparent, true);

		function(a);
		function(b);
	}



	ImageBasedLighting::ImageBasedLighting(RenderScene& scene)
		: SceneRender(scene)
		, AdditiveEffect("shaders/lighting/deferredlighting.vert",
			"shaders/lighting/ibl.frag") 
		, factory(nullptr)
		, effectScale(1.f) {}


	void ImageBasedLighting::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		factory = parameter.factory;
		activateTransparentIBL(true);
		iterIBLShaders(*factory, [this](SceneShader& shader) {
			shader.bind<float>("effectScale", effectScale);
		});

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
			RenderTarget& diffuse  = provider->requestCurrentImage();
			RenderTarget& specular = provider->requestCurrentSpecular();
			LayeredTarget combinedTarget(diffuse, specular);

			parentBuffer->add(combinedTarget);
			parentBuffer->fill(*this, clearNothing);
		}
#else
		AdditiveEffect::fill();
#endif 
	}

	float ImageBasedLighting::getEffectScale() const {
		return effectScale;
	}

	void ImageBasedLighting::setEffectScale(float value) {
		if (effectScale != value && value > 0.f && value <= 1.f) {
			effectScale = value;

			shader.bind<float>("effectScale", effectScale);

			if (factory != nullptr) {
				iterIBLShaders(*factory, [this](SceneShader& shader) {
					shader.bind<float>("effectScale", effectScale);
				});
			}
		}
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

		iterIBLShaders(*factory, [this](SceneShader& shader) {
			if (active) {
				shader.bind<int>("useIBL", 1);
				scene.getLightmanager().addReflectionProbes(shader);
				scene.getLightmanager().bindReflectionProbes(*camera, shader, shader.getSpace());
			}
			else {
				shader.bind<int>("useIBL", 0);
				scene.getLightmanager().removeReflectionProbes(shader);
			}
		});
	}

}