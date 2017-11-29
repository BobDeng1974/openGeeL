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
#include "renderscene.h"
#include "simpleshadowmap.h"

using namespace glm;

namespace geeL {

	SimpleShadowMap::SimpleShadowMap(const Light& light, 
		const ShadowMapConfiguration& config)
			: ShadowMap(light, config.type)
			, shadowBias(config.shadowBias)
			, dynamicBias(config.shadowBias)
			, farPlane(config.farPlane)
			, softShadowResolution(config.softShadowResolution)
			, softShadowScale(config.softShadowScale) {
		
		setResolution(config.resolution);
	}

	void SimpleShadowMap::init() {

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		initFilterMode(FilterMode::Linear);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		setBorderColors(1.f, 1.f, 1.f, 1.f);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Bind depth map to frame buffer (the shadow map)
		glGenFramebuffers(1, &fbo.token);
		FrameBuffer::bind(fbo.token);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);

		//Disable writes to color buffer
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		FrameBuffer::unbind();
	}


	void SimpleShadowMap::bindData(const Shader& shader, const std::string& name) {
		shader.bind<float>(name + "bias", dynamicBias);
		shader.bind<float>(name + "shadowIntensity", intensity);
		shader.bind<int>(name + "resolution", softShadowResolution);
		shader.bind<float>(name + "scale", softShadowScale);
		shader.bind<int>(name + "type", (int)type);
	}


	void SimpleShadowMap::removeMap(Shader& shader) {
		shader.removeMap(*this);
	}



	void SimpleShadowMap::setResolution(ShadowmapResolution resolution) {
		this->resolution = (int)resolution;
	}


	float SimpleShadowMap::getShadowBias() const {
		return dynamicBias;
	}

	void SimpleShadowMap::setShadowBias(float bias) {
		if (bias > 0.f)
			dynamicBias = bias;
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
			: SimpleShadowMap(light, config)
			, spotLight(light) {
	
		init();
	}

	SimpleSpotLightMap::SimpleSpotLightMap(const SpotLight& light, 
		const ShadowMapConfiguration& config, bool init)
			: SimpleShadowMap(light, config)
			, spotLight(light) {}


	void SimpleSpotLightMap::bindData(const Shader& shader, const std::string& name) {
		SimpleShadowMap::bindData(shader, name);

		shader.bind<glm::mat4>(name + "lightTransform", lightTransform);
	}

	void SimpleSpotLightMap::draw(const SceneCamera* const camera, const RenderScene& scene, 
		ShadowmapRepository& repository) {
		
		//Write light transform into shader
		computeLightTransform();

		Viewport::set(0, 0, resolution, resolution);
		FrameBuffer::bind(fbo.token);
		glClear(GL_DEPTH_BUFFER_BIT);

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
		

		FrameBuffer::unbind();
	}

	void SimpleSpotLightMap::draw(const SceneCamera* const camera, const RenderScene& scene,
		const RenderShader& shader) {

		//Write light transform into shader
		computeLightTransform();

		Viewport::set(0, 0, resolution, resolution);
		FrameBuffer::bind(fbo.token);
		glClear(GL_DEPTH_BUFFER_BIT);

		shader.bind<glm::mat4>("lightTransform", lightTransform);

		scene.drawStaticObjects(shader);
		FrameBuffer::unbind();
	}

	TextureType SimpleSpotLightMap::getTextureType() const {
		return TextureType::Texture2D;
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
			: SimpleShadowMap(light, config)
			, pointLight(light) {
	
		lightTransforms.reserve(6);
		computeLightTransform();

		init();
	}


	void SimplePointLightMap::bindData(const Shader& shader, const std::string& name) {
		SimpleShadowMap::bindData(shader, name);

		shader.bind<float>(name + "farPlane", farPlane);
	}

	TextureType SimplePointLightMap::getTextureType() const {
		return TextureType::TextureCube;
	}

	void SimplePointLightMap::init() {

		//Generate depth cube map texture
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		//Write faces of the cubemap
		for (int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		initFilterMode(FilterMode::Linear);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		//Bind depth map to frame buffer (the shadow map)
		glGenFramebuffers(1, &fbo.token);
		FrameBuffer::bind(fbo.token);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, id, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		FrameBuffer::unbind();
	}

	void SimplePointLightMap::draw(const SceneCamera* const camera, const RenderScene& scene,
		ShadowmapRepository& repository) {

		//Write light transforms of cubemap faces into shader
		computeLightTransform();

		Viewport::set(0, 0, resolution, resolution);
		FrameBuffer::bind(fbo.token);
		glClear(GL_DEPTH_BUFFER_BIT);
		
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
		
		FrameBuffer::unbind();
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
			: SimpleShadowMap(light, config)
			, directionalLight(light) {
	
		init();
	}


	void SimpleDirectionalLightMap::bindData(const Shader& shader, const std::string& name) {
		SimpleShadowMap::bindData(shader, name);

		shader.bind<glm::mat4>(name + "lightTransform", lightTransform);
	}

	void SimpleDirectionalLightMap::draw(const SceneCamera* const camera, const RenderScene& scene, 
		ShadowmapRepository& repository) {

		//Write light transform into shader
		computeLightTransform();

		Viewport::set(0, 0, resolution, resolution);
		FrameBuffer::bind(fbo.token);
		glClear(GL_DEPTH_BUFFER_BIT);

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

		
		FrameBuffer::unbind();
	}

	TextureType SimpleDirectionalLightMap::getTextureType() const {
		return TextureType::Texture2D;
	}


	void SimpleDirectionalLightMap::computeLightTransform() {
		float a = resolution / 10.f;
		mat4 projection = ortho(-a, a, -a, a, -farPlane, farPlane);
		mat4 view = lookAt(vec3(0.f), -light.transform.getForwardDirection(), vec3(0.f, 1.f, 0.f));

		lightTransform = projection * view;
	}


}