#define GLEW_STATIC
#include <glew.h>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include "../cameras/camera.h"
#include "../framebuffer/gbuffer.h"
#include "../shader/shader.h"
#include "../transformation/transform.h"
#include "../lighting/spotlight.h"
#include "../lighting/pointlight.h"
#include "../lighting/directionallight.h"
#include "simpleshadowmap.h"

using namespace glm;

namespace geeL {

	SimpleShadowMap::SimpleShadowMap(const Light& light, float shadowBias, float farPlane) 
		: ShadowMap(light), shadowBias(shadowBias), dynamicBias(shadowBias), farPlane(farPlane) {
		
		setResolution(ShadowmapResolution::Adaptive);
	}


	void SimpleShadowMap::init() {

		//Generate depth map texture
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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


	void SimpleShadowMap::bindData(const Shader& shader, const std::string& name) {
		shader.setFloat(name + "bias", dynamicBias);
	}


	void SimpleShadowMap::bindMap(Shader& shader, const std::string& name) {
		shader.addMap(id, name, GL_TEXTURE_2D);
	}

	void SimpleShadowMap::removeMap(Shader& shader) {
		shader.removeMap(id);
	}


	void SimpleShadowMap::adaptShadowmap(const SceneCamera& camera) {
		vec3 center = camera.center;
		//float depth = scene.camera.depth;
		const ScreenInfo& info = *camera.info;
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
			bindShadowmapResolution();
	}

	void SimpleShadowMap::bindShadowmapResolution() const {
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


	SimpleSpotLightMap::SimpleSpotLightMap(const SpotLight& light, float shadowBias, float farPlane)
		: SimpleShadowMap(light, shadowBias, farPlane), spotLight(light) {
	
		init();
	}


	void SimpleSpotLightMap::bindData(const Shader& shader, const std::string& name) {
		shader.setFloat(name + "bias", dynamicBias);
		shader.setMat4(name + "lightTransform", lightTransform);
	}

	void SimpleSpotLightMap::draw(const SceneCamera& camera,
		std::function<void(const Shader&)> renderCall, const Shader& shader) {

		//Write light transform into shader
		computeLightTransform();

		shader.use();
		shader.setMat4("lightTransform", lightTransform);

		if (resolution == ShadowmapResolution::Adaptive)
			adaptShadowmap(camera);

		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		renderCall(shader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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


	SimplePointLightMap::SimplePointLightMap(const PointLight& light, float shadowBias, float farPlane)
		: SimpleShadowMap(light, shadowBias, farPlane), pointLight(light) {
	
		lightTransforms.reserve(6);
		computeLightTransform();

		init();
	}


	void SimplePointLightMap::bindData(const Shader& shader, const std::string& name) {
		shader.setFloat(name + "bias", dynamicBias);
		shader.setFloat(name + "farPlane", farPlane);
	}

	void SimplePointLightMap::bindMap(Shader& shader, const std::string& name) {
		shader.addMap(id, name, GL_TEXTURE_CUBE_MAP);
	}

	void SimplePointLightMap::init() {

		//Generate depth cube map texture
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		//Write faces of the cubemap
		for (int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

	void SimplePointLightMap::draw(const SceneCamera& camera,
		std::function<void(const Shader&)> renderCall, const Shader& shader) {

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

		glViewport(0, 0, width, height);
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

	void SimplePointLightMap::bindShadowmapResolution() const {
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		//Write faces of the cubemap
		for (int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}



	SimpleDirectionalLightMap::SimpleDirectionalLightMap(const DirectionalLight& light, float shadowBias, float farPlane)
		: SimpleShadowMap(light, shadowBias, farPlane), directionalLight(light) {
	
		setResolution(ShadowmapResolution::High);
		init();
	}


	void SimpleDirectionalLightMap::bindData(const Shader& shader, const std::string& name) {
		shader.setFloat(name + "bias", dynamicBias);
		shader.setMat4(name + "lightTransform", lightTransform);
	}

	void SimpleDirectionalLightMap::draw(const SceneCamera& camera,
		std::function<void(const Shader&)> renderCall, const Shader& shader) {

		//Write light transform into shader
		computeLightTransform();

		shader.use();
		shader.setMat4("lightTransform", lightTransform);

		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		renderCall(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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