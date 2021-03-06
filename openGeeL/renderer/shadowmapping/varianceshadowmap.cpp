#define GLEW_STATIC
#include <glew.h>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include "primitives/screenquad.h"
#include "glwrapper/glguards.h"
#include "lights/spotlight.h"
#include "transformation/transform.h"
#include "texturing/texture.h"
#include "renderscene.h"
#include "varianceshadowmap.h"

#if VARIANCE_SHADOWMAP

namespace geeL {

	VarianceSpotLightMap::VarianceSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config)
		: Shadowmap(light, std::unique_ptr<Texture>(new Texture2D(
				Resolution((int)config.resolution),
				ColorType::RG16,
				FilterMode::TrilinearUltra,
				WrapMode::ClampBorder,
				AnisotropicFilter::None)), 
			config.resolution)
		, spotLight(light)
		, shadowBias(config.shadowBias)
		, farPlane(config.farPlane)
		, resolution((int)config.resolution)
		, blur(1.5f, 17)
		, blurTexture(
			Resolution((int)config.resolution), 
			ColorType::RGB16, 
			WrapMode::ClampEdge, 
			FilterMode::Linear) {

		getTexture().mipmap();

		blurBuffer.initResolution(Resolution((int)config.resolution));
		blur.setImage(getTexture());
		blur.init(PostProcessingParameter(ScreenQuad::get(), blurBuffer, Resolution((int)config.resolution)));
	}

	void VarianceSpotLightMap::bind(unsigned int layer) const {
		blurTexture.bind(layer);
	}

	void VarianceSpotLightMap::bindImage(unsigned int position, AccessType access) const {
		blurTexture.bindImage(position, access);
	}


	void VarianceSpotLightMap::draw(const SceneCamera* const camera, const RenderScene& scene,
		ShadowmapRepository& repository) {

		computeLightTransform();

		//Note: currently doesn't work since depth framebuffer is used and texture has different color type
		buffer.add(getTexture());
		buffer.fill([this, &scene, &repository]() {
			if (scene.containsStaticObjects()) {
				const RenderShader& shader = repository.getVariance2DShader();
				shader.bind<glm::mat4>("lightTransform", lightTransform);

				scene.drawGeometry(shader, RenderMode::Static);
			}

			/*
			if (scene.containsSkinnedObjects()) {
				const RenderShader& shader = repository.getSimple2DAnimated();
				shader.bind<glm::mat4>("lightTransform", lightTransform);

				scene.drawGeometry(shader, RenderMode::Skinned);
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

}

#endif