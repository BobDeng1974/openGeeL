#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/type_ptr.hpp>
#include "../shader/shader.h"
#include "../meshes/meshrenderer.h"
#include "../cameras/camera.h"
#include "../transformation/transform.h"
#include "../scene.h"
#include "light.h"

using namespace std;
using namespace glm;

namespace geeL {

	Light::Light(Transform& transform, vec3 diffuse, vec3 specular, float shadowBias)
		: SceneObject(transform), diffuse(diffuse), specular(specular), shadowBias(shadowBias), dynamicBias(shadowBias) {}



	void Light::deferredBind(const RenderScene& scene, const Shader& shader, int index, string name) const {
		forwardBind(shader, index, name);
	}

	void Light::forwardBind(const Shader& shader, int index, string name) const {
		string location = name + "[" + std::to_string(index) + "].";

		shader.setVector3(location + "diffuse", diffuse);
		shader.setVector3(location + "specular", specular);
		shader.setFloat(location + "bias", dynamicBias);
	}

	void Light::initShadowmap() {

		//Generate depth map texture
		glGenTextures(1, &shadowmapID);
		glBindTexture(GL_TEXTURE_2D, shadowmapID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			shadowmapWidth, shadowmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Bind depth map to frame buffer (the shadow map)
		glGenFramebuffers(1, &shadowmapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowmapID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Light::addShadowmap(Shader& shader, string name) {
		shader.addMap(shadowmapID, name, GL_TEXTURE_2D);
	}

	void Light::renderShadowmap(const RenderScene& scene, const Shader& shader) {
		shader.use();

		//Write light transform into shader
		computeLightTransform();
		shader.setMat4("lightTransform", lightTransform);

		if(resolution == ShadowmapResolution::Adaptive)
			adaptShadowmap(scene);

		glViewport(0, 0, shadowmapWidth, shadowmapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		scene.drawObjects(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	float Light::getIntensity(glm::vec3 point) const {
		float distance = length(transform.position - point);

		if (distance == 0.f)
			return 0.f;
		
		return 1.f / (distance * distance);
	}


	void Light::adaptShadowmap(const RenderScene& scene) {
		Transform& trans = scene.camera.transform;
		vec3 camPosition = trans.position;

		float depth = scene.camera.depth;
		vec3 center = camPosition + trans.forward * depth;
		float intensity = 1 - getIntensity(center);

		//Check distance between camera and center pixel of camera 
		//and scale it with experienced intensity at center pixel.
		//Used as heuristic for adaptive resolution
		float distance = intensity * depth;
		bool changed = adaptShadowmapResolution(distance);

		//Only update texture if resolution actually changed
		if (changed)
			bindShadowmapResolution();
	}

	void Light::bindShadowmapResolution() const {
		glBindTexture(GL_TEXTURE_2D, shadowmapID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			shadowmapWidth, shadowmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Light::setResolution(ShadowmapResolution resolution) {
		this->resolution = resolution;

		setDimensions((int)resolution);
	}

	void Light::setDimensions(int resolution) {
		shadowmapWidth = shadowmapHeight = resolution;
	}

	const int Light::getShadowMapID() const {
		return shadowmapID;
	}

	const int Light::getShadowMapFBO() const {
		return shadowmapFBO;
	}
}