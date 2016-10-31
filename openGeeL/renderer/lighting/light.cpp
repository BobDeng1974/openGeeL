#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/type_ptr.hpp>
#include "../shader/shader.h"
#include "../meshes/meshrenderer.h"
#include "../scene.h"
#include "light.h"

using namespace std;
using namespace glm;

namespace geeL {

	void Light::deferredBind(const Shader& shader, int index, string name) const {
		std::string location = name + "[" + std::to_string(index) + "].";

		shader.setVector3(location + "diffuse", diffuse);
		shader.setVector3(location + "specular", specular);
		shader.setVector3(location + "ambient", ambient);

		shader.setFloat(location + "intensity", intensity);
		shader.setFloat(location + "bias", shadowBias);
	}

	void Light::forwardBind(const Shader& shader, int index, string name) const {
		deferredBind(shader, index, name);
	}

	void Light::initShadowmap() {
		shadowmapHeight = shadowmapWidth = 512;

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

		glViewport(0, 0, shadowmapWidth, shadowmapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		scene.drawObjects(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	const int Light::getShadowMapID() const {
		return shadowmapID;
	}

	const int Light::getShadowMapFBO() const {
		return shadowmapFBO;
	}
}