#define GLEW_STATIC
#include <glew.h>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include "cameras/camera.h"
#include "glwrapper/viewport.h"
#include "shader/rendershader.h"
#include "transformation/transform.h"
#include "lights/spotlight.h"
#include "lights/pointlight.h"
#include "lights/directionallight.h"
#include "framebuffer/framebuffer.h"
#include "texturing/texture.h"
#include "renderscene.h"
#include "simpleshadowmap.h"

using namespace glm;

namespace geeL {

	SimpleShadowmap::SimpleShadowmap(const Light& light, 
		std::unique_ptr<Texture> innerTexture,
		const ShadowMapConfiguration& config)
			: Shadowmap(light, std::move(innerTexture), config.resolution)
			, type(config.type)
			, shadowBias(config.shadowBias)
			, farPlane(config.farPlane)
			, softShadowResolution(config.softShadowResolution)
			, softShadowScale(config.softShadowScale) {
		
		setResolution(config.resolution);
	}


	void SimpleShadowmap::draw(const SceneCamera* const camera, 
		const RenderScene& scene, 
		ShadowmapRepository& repository) {

		computeLightTransform();

		buffer.add(getTexture());
		buffer.fill([this, &scene, &repository]() {
			this->drawMap(scene, repository);
		});
	}

	void SimpleShadowmap::bindData(const Shader& shader, const std::string& name) {
		shader.bind<float>(name + "bias", shadowBias);
		shader.bind<float>(name + "shadowIntensity", intensity);
		shader.bind<int>(name + "resolution", softShadowResolution);
		shader.bind<float>(name + "scale", softShadowScale);
		shader.bind<int>(name + "type", (int)type);
	}

	ShadowMapType SimpleShadowmap::getType() const {
		return type;
	}

	void SimpleShadowmap::setResolution(ShadowmapResolution resolution) {
		this->resolution = (int)resolution;
	}

	Resolution SimpleShadowmap::getScreenResolution() const {
		return getTexture().getScreenResolution();
	}

	float SimpleShadowmap::getShadowBias() const {
		return shadowBias;
	}

	void SimpleShadowmap::setShadowBias(float bias) {
		if (bias > 0.f)
			shadowBias = bias;
	}

	int SimpleShadowmap::getSoftShadowResolution() const {
		return softShadowResolution;
	}

	void SimpleShadowmap::setSoftShadowResolution(unsigned int resolution) {
		if (resolution != softShadowResolution && resolution < 100)
			softShadowResolution = resolution;
	}

	float SimpleShadowmap::getSoftShadowScale() const {
		return softShadowScale;
	}

	void SimpleShadowmap::setSoftShadowScale(float scale) {
		if (scale != softShadowScale && scale > 0.f)
			softShadowScale = scale;
	}

	float SimpleShadowmap::getFarPlane() const {
		return farPlane;
	}

	void SimpleShadowmap::setFarPlane(float value) {
		if (farPlane != value && value > 0.f) {
			farPlane = value;
		}
	}



	SimpleSpotLightMap::SimpleSpotLightMap(const SpotLight& light, 
		const ShadowMapConfiguration& config)
			: SimpleShadowmap(light, 
				std::unique_ptr<Texture>(new Texture2D(
					Resolution((int)config.resolution),
					ColorType::Depth,
					FilterMode::Linear,
					WrapMode::ClampBorder,
					AnisotropicFilter::None)),
				config)
			, spotLight(light) {}

	SimpleSpotLightMap::SimpleSpotLightMap(const SpotLight& light, 
		const ShadowMapConfiguration& config, bool init)
			: SimpleShadowmap(light, 
				std::unique_ptr<Texture>(new Texture2D(
					Resolution((int)config.resolution), 
					ColorType::Depth,
					FilterMode::Linear,
					WrapMode::ClampBorder,
					AnisotropicFilter::None)),
				config)
			, spotLight(light) {}


	void SimpleSpotLightMap::bindData(const Shader& shader, const std::string& name) {
		SimpleShadowmap::bindData(shader, name);

		shader.bind<glm::mat4>(name + "lightTransform", lightTransform);
	}

	void SimpleSpotLightMap::drawMap(const RenderScene& scene, ShadowmapRepository& repository) {
		const RenderShader& staticShader = repository.getSimple2DShader();
		const RenderShader& skinnedShader = repository.getSimple2DAnimated();

		staticShader.bind<glm::mat4>("lightTransform", lightTransform);
		skinnedShader.bind<glm::mat4>("lightTransform", lightTransform);

		scene.drawGeometry(staticShader, skinnedShader, &frustum);
	}

	void SimpleSpotLightMap::computeLightTransform() {
		Transform& transform = light.transform;

		mat4& projection = glm::perspective(spotLight.getAngleDegree() * 2.f, 1.f, 1.f, farPlane);
		mat4& view = lookAt(transform.getPosition(), transform.getPosition() +
			transform.getForwardDirection(), transform.getUpDirection());

		lightTransform = projection * view;

		frustum.setParameters(projection);
		frustum.update(transform);
	}




	SimplePointLightMap::SimplePointLightMap(const PointLight& light, 
		const ShadowMapConfiguration& config)
			: SimpleShadowmap(light, 
				std::unique_ptr<Texture>(new TextureCube(
					(int)config.resolution, 
					ColorType::Depth,
					FilterMode::Linear,
					WrapMode::ClampEdge)),
				config)
			, pointLight(light) {
	
		lightTransforms.reserve(6);
		computeLightTransform();
	}


	void SimplePointLightMap::bindData(const Shader& shader, const std::string& name) {
		SimpleShadowmap::bindData(shader, name);

		shader.bind<float>(name + "farPlane", farPlane);
	}

	void SimplePointLightMap::drawMap(const RenderScene& scene, ShadowmapRepository& repository) {
		const RenderShader& staticShader = repository.getSimpleCubeShader();
		const RenderShader& skinnedShader = repository.getSimpleCubeAnimated();

		for (int i = 0; i < 6; i++) {
			std::string name = "lightTransforms[" + std::to_string(i) + "]";

			staticShader.bind<glm::mat4>(name, lightTransforms[i]);
			skinnedShader.bind<glm::mat4>(name, lightTransforms[i]);
		}

		staticShader.bind<float>("farPlane", farPlane);
		skinnedShader.bind<float>("farPlane", farPlane);

		staticShader.bind<glm::vec3>("lightPosition", light.transform.getPosition());
		skinnedShader.bind<glm::vec3>("lightPosition", light.transform.getPosition());

		scene.drawGeometry(staticShader, skinnedShader);
	}

	void SimplePointLightMap::computeLightTransform() {
		mat4&& projection = glm::perspective(90.f, 1.f, 1.f, farPlane);

		Transform& transform = light.transform;
		mat4&& view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[0] = projection * view;

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(-1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[1] = projection * view;

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f));
		lightTransforms[2] = projection * view;

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, -1.f, 0.f), vec3(0.f, 0.f, -1.f));
		lightTransforms[3] = projection * view;

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 0.f, 1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[4] = projection * view;

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 0.f, -1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[5] = projection * view;
	}


	SimpleDirectionalLightMap::SimpleDirectionalLightMap(const DirectionalLight& light, 
		const ShadowMapConfiguration& config)
			: SimpleShadowmap(light, 
				std::unique_ptr<Texture>(new Texture2D(
					Resolution((int)config.resolution),
					ColorType::Depth,
					FilterMode::Linear,
					WrapMode::ClampBorder,
					AnisotropicFilter::None)),
				config)
			, directionalLight(light) {}


	void SimpleDirectionalLightMap::bindData(const Shader& shader, const std::string& name) {
		SimpleShadowmap::bindData(shader, name);

		shader.bind<glm::mat4>(name + "lightTransform", lightTransform);
	}

	void SimpleDirectionalLightMap::drawMap(const RenderScene& scene, ShadowmapRepository& repository) {
		const RenderShader& staticShader = repository.getSimple2DShader();
		const RenderShader& skinnedShader = repository.getSimple2DAnimated();

		staticShader.bind<glm::mat4>("lightTransform", lightTransform);
		skinnedShader.bind<glm::mat4>("lightTransform", lightTransform);

		scene.drawGeometry(staticShader, skinnedShader, &frustum);
	}

	void SimpleDirectionalLightMap::computeLightTransform() {
		float a = farPlane;
		vec3 position = light.transform.getPosition();
		mat4 projection = ortho(-a, a, -a, a, -a, a);
		mat4 view = lookAt(position, position - light.transform.getForwardDirection(), vec3(0.f, 1.f, 0.f));

		lightTransform = projection * view;

		frustum.setParameters(projection);
		frustum.update(position, position - light.transform.getForwardDirection(), vec3(0.f, 1.f, 0.f));
	}

}