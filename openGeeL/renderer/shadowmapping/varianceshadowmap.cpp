#define GLEW_STATIC
#include <glew.h>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include "primitives/screenquad.h"
#include "glwrapper/glguards.h"
#include "lights/spotlight.h"
#include "transformation/transform.h"
#include "renderscene.h"
#include "varianceshadowmap.h"

namespace geeL {

	VarianceSpotLightMap::VarianceSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config)
		: ShadowMap(light, std::unique_ptr<Texture>(new RenderTexture(
			Resolution((int)config.resolution),
			ColorType::RG16,
			WrapMode::ClampBorder,
			FilterMode::TrilinearUltra)))
		, spotLight(light)
		, shadowBias(config.shadowBias)
		, farPlane(config.farPlane)
		, resolution((int)config.resolution)
		, temp(*this, Resolution((int)config.resolution))
		, blur(KernelSize::Depth, 1.5f)
		, blurTexture(
			Resolution((int)config.resolution), 
			ColorType::RGB16, 
			WrapMode::ClampEdge, 
			FilterMode::Linear) {

		getInnerTexture().mipmap();

		blurBuffer.initResolution(Resolution((int)config.resolution));
		blur.setImage(temp);
		blur.init(PostProcessingParameter(ScreenQuad::get(), blurBuffer, Resolution((int)config.resolution)));
	}


	void VarianceSpotLightMap::draw(const SceneCamera* const camera, const RenderScene& scene,
		ShadowmapRepository& repository) {

		computeLightTransform();

		//Note: currently doesn't work since depth framebuffer is used
		buffer.add(getInnerTexture());
		buffer.fill([this, &scene, &repository]() {
			if (scene.containsStaticObjects()) {
				const RenderShader& shader = repository.getVariance2DShader();
				shader.bind<glm::mat4>("lightTransform", lightTransform);

				scene.drawStaticObjects(shader);
			}

			/*
			if (scene.containsSkinnedObjects()) {
				const RenderShader& shader = repository.getSimple2DAnimated();
				shader.bind<glm::mat4>("lightTransform", lightTransform);

				scene.drawSkinnedObjects(shader);
			}
			*/
		});
		
		//Blur shadow map
		DepthGuard depthGuard(true);

		glCullFace(GL_FRONT);
		blurBuffer.push(blurTexture);
		blur.bindValues();
		blurBuffer.fill(blur);
		glCullFace(GL_BACK);

	}

	void VarianceSpotLightMap::computeLightTransform() {
		Transform& transform = light.transform;

		float fov = glm::degrees(spotLight.getAngle());
		mat4&& projection = glm::perspective(fov, 1.f, 1.f, farPlane);
		mat4&& view = glm::lookAt(transform.getPosition(), transform.getPosition() +
			transform.getForwardDirection(), transform.getUpDirection());

		lightTransform = projection * view;
	}

	void VarianceSpotLightMap::bindData(const Shader& shader, const std::string& name) {
		shader.bind<float>(name + "bias", shadowBias);
		shader.bind<float>(name + "shadowIntensity", intensity);
	}

	Resolution VarianceSpotLightMap::getScreenResolution() const {
		return Resolution(resolution);
	}

	unsigned int VarianceSpotLightMap::getID() const {
		return blurTexture.getID();
	}
}