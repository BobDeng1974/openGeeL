#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <algorithm>
#include <limits>
#include <vector>
#include <vec3.hpp>
#include <vec4.hpp>
#include "../shader/shader.h"
#include "../cameras/camera.h"
#include "../lighting/light.h"
#include "../transformation/transform.h"
#include "cascadedmap.h"
#include <iostream>

namespace geeL {

	CascadedDirectionalShadowMap::CascadedDirectionalShadowMap(const Light& light, const Camera& camera, 
		float shadowBias, unsigned int width, unsigned int height)
			: CascadedShadowMap(light, shadowBias, width, height) {
		
		unsigned int IDs[MAPCOUNT];
		glGenTextures(MAPCOUNT, IDs);
		for (unsigned int i = 0; i < MAPCOUNT; i++) {
			glBindTexture(GL_TEXTURE_2D, IDs[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 
				0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, IDs[0], 0);

		//Disable writing to color buffer
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		for (unsigned int i = 0; i < MAPCOUNT; i++)
			shadowMaps[i].id = IDs[i];

		setCascades(camera);
	}


	void CascadedDirectionalShadowMap::setCascades(const Camera& camera) {
		float step = 1.f / (float)MAPCOUNT;
		float near = camera.getNearPlane();
		float far = camera.getFarPlane();

		for (unsigned int i = 0; i < MAPCOUNT; i++) {
			float s = (i + 1.f) * step;

			float cascadeEnd = s * far + (1.f - s) * near;
			shadowMaps[i].cascadeEnd = cascadeEnd;

			glm::vec4 cascadeClip = camera.getProjectionMatrix() * glm::vec4(0.f, 0.f, cascadeEnd, 1.f);
			shadowMaps[i].cascadeEndClip = cascadeClip.z;
		}
	}

	void CascadedDirectionalShadowMap::bindData(const Shader& shader, const std::string& name) {
		shader.setFloat(name + "bias", shadowBias);

		for (unsigned int i = 0; i < MAPCOUNT; i++) {
			shader.setMat4(name + "lightTransforms[" + std::to_string(i) + "]", shadowMaps[i].lightTransform);
			shader.setFloat(name + "cascadeEndClip[" + std::to_string(i) + "]", shadowMaps[i].cascadeEndClip);
		}
	}

	void CascadedDirectionalShadowMap::bindMap(Shader& shader, const std::string& name) {
		for (unsigned int i = 0; i < MAPCOUNT; i++)
			shader.addMap(shadowMaps[i].id, name + "s[" + std::to_string(i) + "]");
	}


	void CascadedDirectionalShadowMap::draw(const Camera& camera,
		std::function<void(const Shader&)> renderCall, const Shader& shader) {

		computeLightTransforms(camera);

		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

		shader.use();
		for (unsigned int i = 0; i < MAPCOUNT; i++) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMaps[i].id, 0);
			glClear(GL_DEPTH_BUFFER_BIT);

			shader.setMat4("lightTransform", shadowMaps[i].lightTransform);
			renderCall(shader);
		}


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void CascadedDirectionalShadowMap::computeLightTransforms(const Camera& camera) {

		float near = camera.getNearPlane();
		for (unsigned int i = 0; i < MAPCOUNT; i++) {
			float far = shadowMaps[i].cascadeEnd;
			
			//Initialize bounding box borders with infinite cube
			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();

			glm::mat4 lightView = glm::lookAt(light.transform.getForwardDirection() * (far * 0.5f),
				vec3(0.f), vec3(0.f, 1.f, 0.f));

			std::vector<glm::vec3>&& borders = camera.getViewBorders(near, far);
			for (auto it = borders.begin(); it != borders.end(); it++) {
				//Transform borders into world and then into light space coordinates
				glm::vec4 border = lightView * camera.getInverseViewMatrix() * glm::vec4(*it, 1.f);

				//Enclose bounding box
				minX = std::min(minX, border.x);
				maxX = std::max(maxX, border.x);
				minY = std::min(minY, border.y);
				maxY = std::max(maxY, border.y);
				minZ = std::min(minZ, border.z);
				maxZ = std::max(maxZ, border.z);
			}

			glm::mat4&& lightProj = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
			shadowMaps[i].lightTransform = lightProj * lightView;

			near = far;
		}
	}

	unsigned int CascadedDirectionalShadowMap::getID() const {
		return shadowMaps[0].id;
	}

}