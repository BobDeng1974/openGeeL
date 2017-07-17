#define GLEW_STATIC
#include <glew.h>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include "../cameras/camera.h"
#include "../utility/screeninfo.h"
#include "../utility/viewport.h"
#include "../shader/rendershader.h"
#include "../transformation/transform.h"
#include "../lights/spotlight.h"
#include "../lights/pointlight.h"
#include "../lights/directionallight.h"
#include "simpleshadowmap.h"
#include <iostream>

using namespace glm;

namespace geeL {

	SimpleShadowMap::SimpleShadowMap(const Light& light, const ShadowMapConfiguration& config)
		: ShadowMap(light, config.type), shadowBias(config.shadowBias), dynamicBias(config.shadowBias), farPlane(config.farPlane), 
			softShadowResolution(config.softShadowResolution), softShadowScale(config.softShadowScale) {
		
		setResolution(config.resolution);
	}

	SimpleShadowMap::~SimpleShadowMap() {
		remove();
	}


	void SimpleShadowMap::init() {

		//Generate depth map texture
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		initFilterMode(FilterMode::Linear);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Bind depth map to frame buffer (the shadow map)
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);

		//Disable writes to color buffer
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void SimpleShadowMap::bindData(const RenderShader& shader, const std::string& name) {
		shader.setFloat(name + "bias", dynamicBias);
		shader.setFloat(name + "shadowIntensity", intensity);
		shader.setInteger(name + "resolution", softShadowResolution);
		shader.setFloat(name + "scale", softShadowScale);
		shader.setInteger(name + "type", (int)type);
	}


	void SimpleShadowMap::removeMap(RenderShader& shader) {
		shader.removeMap(*this);
	}

	void SimpleShadowMap::remove() {
		glDeleteTextures(1, &id);
		glDeleteFramebuffers(1, &fbo);
	}

	void SimpleShadowMap::adaptShadowmap(const SceneCamera* const camera) {

		//Fallback strategy if no camera was forwarded
		if (camera == nullptr) {
			//Draw with fixed resolution
			width = height = 512;
			bindShadowmapResolution(width, height);
			return;
		}

		vec3 center = camera->center;
		//float depth = scene.camera.depth;
		const ScreenInfo& info = camera->info;
		float depth = fminf(info.CTdepth, fminf(info.BLdepth,
			fminf(info.BRdepth, fminf(info.TLdepth, info.TRdepth))));

		float intensity = 1.f - light.getIntensity(center);

		//Check distance between camera and center pixel of camera 
		//and scale it with experienced intensity at center pixel.
		//Used as heuristic for adaptive resolution
		float distance = intensity * depth;
		bool changed = adaptShadowmapResolution(distance);

		//Only update texture if resolution actually changed
		if (changed)
			bindShadowmapResolution(width, height);
	}

	void SimpleShadowMap::bindShadowmapResolution(unsigned int width, unsigned int height) const {
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void SimpleShadowMap::setResolution(ShadowmapResolution resolution) {
		this->resolution = resolution;

		width = height = (int)resolution;
	}

	bool SimpleShadowMap::setResolution(int resolution, float biasFactor) {
		if (width != resolution) {
			width = height = (int)resolution;
			dynamicBias = shadowBias * biasFactor;
			return true;
		}

		return false;
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


	SimpleSpotLightMap::SimpleSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config)
		: SimpleShadowMap(light, config), spotLight(light) {
	
		init();
	}


	void SimpleSpotLightMap::bindData(const RenderShader& shader, const std::string& name) {
		SimpleShadowMap::bindData(shader, name);

		shader.setMat4(name + "lightTransform", lightTransform);
	}

	void SimpleSpotLightMap::draw(const SceneCamera* const camera,
		std::function<void(const RenderShader&)> renderCall, const RenderShader& shader) {

		//Write light transform into shader
		computeLightTransform();

		shader.use();
		shader.setMat4("lightTransform", lightTransform);

		if (resolution == ShadowmapResolution::Adaptive)
			adaptShadowmap(camera);

		Viewport::set(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		renderCall(shader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	TextureType SimpleSpotLightMap::getTextureType() const {
		return TextureType::Texture2D;
	}

	void SimpleSpotLightMap::computeLightTransform() {
		Transform& transform = light.transform;

		float fov = glm::degrees(spotLight.getAngle());
		mat4&& projection = glm::perspective(fov, 1.f, 1.0f, farPlane);
		mat4&& view = lookAt(transform.getPosition(), transform.getPosition() +
			transform.getForwardDirection(), transform.getUpDirection());

		lightTransform = projection * view;
	}

	bool SimpleSpotLightMap::adaptShadowmapResolution(float distance) {
		if (distance < 1.f)
			return setResolution(1024, 0.5f);
		else if (distance < 6.f)
			return setResolution(768, 0.5f);
		else if (distance < 10.f)
			return setResolution(512, 0.5f);
		else if (distance < 15.f)
			return setResolution(256, 1.f);
		else
			return setResolution(128, 1.5f);

		return false;
	}


	SimplePointLightMap::SimplePointLightMap(const PointLight& light, const ShadowMapConfiguration& config)
		: SimpleShadowMap(light, config), pointLight(light) {
	
		lightTransforms.reserve(6);
		computeLightTransform();

		init();
	}


	void SimplePointLightMap::bindData(const RenderShader& shader, const std::string& name) {
		SimpleShadowMap::bindData(shader, name);

		shader.setFloat(name + "farPlane", farPlane);
	}

	TextureType SimplePointLightMap::getTextureType() const {
		return TextureType::TextureCube;
	}

	void SimplePointLightMap::init() {

		//Generate depth cube map texture
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		//Write faces of the cubemap
		for (int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		initFilterMode(FilterMode::Linear);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		//Bind depth map to frame buffer (the shadow map)
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, id, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SimplePointLightMap::draw(const SceneCamera* const camera,
		std::function<void(const RenderShader&)> renderCall, const RenderShader& shader) {

		//Write light transforms of cubemap faces into shader
		computeLightTransform();

		shader.use();
		for (int i = 0; i < 6; i++) {
			std::string name = "lightTransforms[" + std::to_string(i) + "]";
			shader.setMat4(name, lightTransforms[i]);
		}

		shader.setFloat("farPlane", farPlane);
		shader.setVector3("lightPosition", light.transform.getPosition());

		if (resolution == ShadowmapResolution::Adaptive)
			adaptShadowmap(camera);

		Viewport::set(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		renderCall(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	bool SimplePointLightMap::adaptShadowmapResolution(float distance) {
		if (distance < 4.f)
			return setResolution(1024, 0.2f);
		else if (distance < 10.f)
			return setResolution(768, 0.3f);
		else if (distance < 15.f)
			return setResolution(512, 0.4f);
		else if (distance < 20.f)
			return setResolution(386, 0.5f);
		else
			return setResolution(256, 0.6f);

		return false;
	}

	void SimplePointLightMap::bindShadowmapResolution(unsigned int width, unsigned int height) const {
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		//Write faces of the cubemap
		for (int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}



	SimpleDirectionalLightMap::SimpleDirectionalLightMap(const DirectionalLight& light, const ShadowMapConfiguration& config)
		: SimpleShadowMap(light, config), directionalLight(light) {
	
		setResolution(ShadowmapResolution::High);
		init();
	}


	void SimpleDirectionalLightMap::bindData(const RenderShader& shader, const std::string& name) {
		SimpleShadowMap::bindData(shader, name);

		shader.setMat4(name + "lightTransform", lightTransform);
	}

	void SimpleDirectionalLightMap::draw(const SceneCamera* const camera,
		std::function<void(const RenderShader&)> renderCall, const RenderShader& shader) {

		//Write light transform into shader
		computeLightTransform();

		shader.use();
		shader.setMat4("lightTransform", lightTransform);

		Viewport::set(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		renderCall(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	TextureType SimpleDirectionalLightMap::getTextureType() const {
		return TextureType::Texture2D;
	}


	void SimpleDirectionalLightMap::computeLightTransform() {
		float a = width / 10.f;
		mat4 projection = ortho(-a, a, -a, a, -farPlane, farPlane);
		mat4 view = lookAt(vec3(0.f), -light.transform.getForwardDirection(), vec3(0.f, 1.f, 0.f));

		lightTransform = projection * view;
	}

	bool SimpleDirectionalLightMap::adaptShadowmapResolution(float distance) {
		return false;
	}

}