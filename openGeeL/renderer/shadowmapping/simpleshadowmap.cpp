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

	SimpleShadowMap::SimpleShadowMap(const Light& light, 
		std::unique_ptr<Texture> innerTexture,
		const ShadowMapConfiguration& config)
			: ShadowMap(light, std::move(innerTexture), config.resolution)
			, type(config.type)
			, shadowBias(config.shadowBias)
			, farPlane(config.farPlane)
			, softShadowResolution(config.softShadowResolution)
			, softShadowScale(config.softShadowScale) {
		
		setResolution(config.resolution);
	}


	void SimpleShadowMap::draw(const SceneCamera* const camera, 
		const RenderScene& scene, 
		ShadowmapRepository& repository) {

		computeLightTransform();

		buffer.add(getTexture());
		buffer.fill([this, &scene, &repository]() {
			this->drawMap(scene, repository);
		});
	}

	void SimpleShadowMap::bindData(const Shader& shader, const std::string& name) {
		shader.bind<float>(name + "bias", shadowBias);
		shader.bind<float>(name + "shadowIntensity", intensity);
		shader.bind<int>(name + "resolution", softShadowResolution);
		shader.bind<float>(name + "scale", softShadowScale);
		shader.bind<int>(name + "type", (int)type);
	}

	ShadowMapType SimpleShadowMap::getType() const {
		return type;
	}

	void SimpleShadowMap::setResolution(ShadowmapResolution resolution) {
		this->resolution = (int)resolution;
	}

	Resolution SimpleShadowMap::getScreenResolution() const {
		return Resolution(resolution);
	}

	float SimpleShadowMap::getShadowBias() const {
		return shadowBias;
	}

	void SimpleShadowMap::setShadowBias(float bias) {
		if (bias > 0.f)
			shadowBias = bias;
	}

	int SimpleShadowMap::getSoftShadowResolution() const {
		return softShadowResolution;
	}

	void SimpleShadowMap::setSoftShadowResolution(unsigned int resolution) {
		if (resolution != softShadowResolution && resolution < 100)
			softShadowResolution = resolution;
	}

	float SimpleShadowMap::getSoftShadowScale() const {
		return softShadowScale;
	}

	void SimpleShadowMap::setSoftShadowScale(float scale) {
		if (scale != softShadowScale && scale > 0.f)
			softShadowScale = scale;
	}



	SimpleSpotLightMap::SimpleSpotLightMap(const SpotLight& light, 
		const ShadowMapConfiguration& config)
			: SimpleShadowMap(light, 
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
			: SimpleShadowMap(light, 
				std::unique_ptr<Texture>(new Texture2D(
					Resolution((int)config.resolution), 
					ColorType::Depth,
					FilterMode::Linear,
					WrapMode::ClampBorder,
					AnisotropicFilter::None)),
				config)
			, spotLight(light) {}


	void SimpleSpotLightMap::bindData(const Shader& shader, const std::string& name) {
		SimpleShadowMap::bindData(shader, name);

		shader.bind<glm::mat4>(name + "lightTransform", lightTransform);
	}

	void SimpleSpotLightMap::drawMap(const RenderScene& scene, ShadowmapRepository& repository) {
		if (scene.containsStaticObjects()) {
			const RenderShader& shader = repository.getSimple2DShader();
			shader.bind<glm::mat4>("lightTransform", lightTransform);

			scene.drawStaticObjects(shader);
		}

		if (scene.containsSkinnedObjects()) {
			const RenderShader& shader = repository.getSimple2DAnimated();
			shader.bind<glm::mat4>("lightTransform", lightTransform);

			scene.drawSkinnedObjects(shader);
		}
	}

	void SimpleSpotLightMap::computeLightTransform() {
		Transform& transform = light.transform;

		float fov = glm::degrees(spotLight.getAngle());
		mat4&& projection = glm::perspective(fov, 1.f, 1.f, farPlane);
		mat4&& view = lookAt(transform.getPosition(), transform.getPosition() +
			transform.getForwardDirection(), transform.getUpDirection());

		lightTransform = projection * view;
	}




	SimplePointLightMap::SimplePointLightMap(const PointLight& light, 
		const ShadowMapConfiguration& config)
			: SimpleShadowMap(light, 
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
		SimpleShadowMap::bindData(shader, name);

		shader.bind<float>(name + "farPlane", farPlane);
	}

	void SimplePointLightMap::drawMap(const RenderScene& scene, ShadowmapRepository& repository) {
		if (scene.containsStaticObjects()) {
			const RenderShader& shader = repository.getSimpleCubeShader();
			for (int i = 0; i < 6; i++) {
				std::string name = "lightTransforms[" + std::to_string(i) + "]";
				shader.bind<glm::mat4>(name, lightTransforms[i]);
			}

			shader.bind<float>("farPlane", farPlane);
			shader.bind<glm::vec3>("lightPosition", light.transform.getPosition());

			scene.drawStaticObjects(shader);
		}

		if (scene.containsSkinnedObjects()) {
			const RenderShader& shader = repository.getSimpleCubeAnimated();
			for (int i = 0; i < 6; i++) {
				std::string name = "lightTransforms[" + std::to_string(i) + "]";
				shader.bind<glm::mat4>(name, lightTransforms[i]);
			}

			shader.bind<float>("farPlane", farPlane);
			shader.bind<glm::vec3>("lightPosition", light.transform.getPosition());

			scene.drawSkinnedObjects(shader);
		}
	}

	void SimplePointLightMap::computeLightTransform() {
		mat4&& projection = glm::perspective(90.f, 1.f, 1.0f, farPlane);

		Transform& transform = light.transform;
		mat4&& view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[0] = std::move(projection * view);

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(-1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[1] = std::move(projection * view);

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f));
		lightTransforms[2] = std::move(projection * view);

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, -1.f, 0.f), vec3(0.f, 0.f, -1.f));
		lightTransforms[3] = std::move(projection * view);

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 0.f, 1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[4] = std::move(projection * view);

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 0.f, -1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[5] = std::move(projection * view);
	}


	SimpleDirectionalLightMap::SimpleDirectionalLightMap(const DirectionalLight& light, 
		const ShadowMapConfiguration& config)
			: SimpleShadowMap(light, 
				std::unique_ptr<Texture>(new Texture2D(
					Resolution((int)config.resolution),
					ColorType::Depth,
					FilterMode::Linear,
					WrapMode::ClampBorder,
					AnisotropicFilter::None)),
				config)
			, directionalLight(light) {}


	void SimpleDirectionalLightMap::bindData(const Shader& shader, const std::string& name) {
		SimpleShadowMap::bindData(shader, name);

		shader.bind<glm::mat4>(name + "lightTransform", lightTransform);
	}

	void SimpleDirectionalLightMap::drawMap(const RenderScene& scene, ShadowmapRepository& repository) {
		if (scene.containsStaticObjects()) {
			const RenderShader& shader = repository.getSimple2DShader();
			shader.bind<glm::mat4>("lightTransform", lightTransform);

			scene.drawStaticObjects(shader);
		}

		if (scene.containsSkinnedObjects()) {
			const RenderShader& shader = repository.getSimple2DAnimated();
			shader.bind<glm::mat4>("lightTransform", lightTransform);

			scene.drawSkinnedObjects(shader);
		}
	}

	void SimpleDirectionalLightMap::computeLightTransform() {
		float a = farPlane;
		vec3 position = light.transform.getPosition();
		mat4 projection = ortho(-a, a, -a, a, -a, a);
		mat4 view = lookAt(position, position - light.transform.getForwardDirection(), vec3(0.f, 1.f, 0.f));

		lightTransform = projection * view;
	}

}