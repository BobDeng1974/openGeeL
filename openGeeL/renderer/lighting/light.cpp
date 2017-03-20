#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/type_ptr.hpp>
#include "../shader/shader.h"
#include "../meshes/meshrenderer.h"
#include "../cameras/camera.h"
#include "../transformation/transform.h"
#include "../utility/gbuffer.h"
#include "../scene.h"
#include "light.h"

using namespace std;
using namespace glm;

namespace geeL {

	Light::Light(Transform& transform, vec3 diffuse, float shadowBias, std::string name)
		: SceneObject(transform, name), diffuse(diffuse), shadowBias(shadowBias), dynamicBias(shadowBias) {}


	void Light::deferredBind(const RenderScene& scene, const Shader& shader, string name) const {
		forwardBind(shader, name, "");
	}

	void Light::forwardBind(const Shader& shader,  string name, string transformName) const {
		shader.setVector3(name + "diffuse", diffuse);
		shader.setFloat(name + "bias", dynamicBias);
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
		//Write light transform into shader
		computeLightTransform();

		shader.use();
		shader.setMat4("lightTransform", lightTransform);

		if(resolution == ShadowmapResolution::Adaptive)
			adaptShadowmap(scene);

		glViewport(0, 0, shadowmapWidth, shadowmapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		//scene.drawObjects(shader);
		scene.iterRenderObjects([&](const MeshRenderer* object) {
			if (object->isActive())
				object->draw(shader);
		});

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	float Light::getIntensity(glm::vec3 point) const {
		float distance = length(transform.getPosition() - point);

		if (distance == 0.f)
			return 0.f;
		
		return 1.f / (distance * distance);
	}


	void Light::adaptShadowmap(const RenderScene& scene) {
		vec3 center = scene.camera.center;
		//float depth = scene.camera.depth;
		const ScreenInfo& info = *scene.camera.info;
		float depth = fminf(info.CTdepth, fminf(info.BLdepth,
			fminf(info.BRdepth, fminf(info.TLdepth, info.TRdepth))));

		float intensity = 1.f - getIntensity(center);

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

	float Light::getShadowBias() const {
		return dynamicBias;
	}

	void Light::setShadowBias(float bias) {
		if(bias > 0.f)
			dynamicBias = bias;
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

	const mat4& Light::getLightTransform() const {
		return lightTransform;
	}
}