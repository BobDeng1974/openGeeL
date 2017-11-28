#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <algorithm>
#include <limits>
#include <vector>
#include <vec3.hpp>
#include <vec4.hpp>
#include "shader/rendershader.h"
#include "cameras/camera.h"
#include "lights/light.h"
#include "utility/viewport.h"
#include "transformation/transform.h"
#include "framebuffer/framebuffer.h"
#include "renderscene.h"
#include "cascadedmap.h"

namespace geeL {

	CascadedDirectionalShadowMap::CascadedDirectionalShadowMap(const Light& light, const SceneCamera& camera, 
		float shadowBias, unsigned int width, unsigned int height)
			: CascadedShadowMap(light, shadowBias, width, height) {
		
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 
			0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		initFilterMode(FilterMode::Linear);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glGenFramebuffers(1, &fbo.token);
		FrameBuffer::bind(fbo.token);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);

		//Disable writing to color buffer
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		FrameBuffer::unbind();

		setCascades(camera);
	}


	void CascadedDirectionalShadowMap::setCascades(const SceneCamera& camera) {
		float near = camera.getNearPlane();
		float far = camera.getFarPlane();

		for (unsigned int i = 0; i < MAPCOUNT; i++) {
			float s = ((float)i / (float)MAPCOUNT);
			s *= s; //Quadratic mapping of distances

			float cascadeEnd = s * far + (1.f - s) * near;
			shadowMaps[i].cascadeEnd = cascadeEnd;

			glm::vec4 cascadeClip = camera.getProjectionMatrix() * glm::vec4(0.f, 0.f, cascadeEnd, 1.f);
			shadowMaps[i].cascadeEndClip = cascadeClip.z;
		}
	}

	void CascadedDirectionalShadowMap::bindData(const Shader& shader, const std::string& name) {
		shader.bind<float>(name + "bias", shadowBias);
		shader.bind<int>(name + "type", (int)type);

		for (unsigned int i = 0; i < MAPCOUNT; i++) {
			shader.bind<glm::mat4>(name + "lightTransforms[" + std::to_string(i) + "]", shadowMaps[i].lightTransform);
			shader.bind<float>(name + "cascadeEndClip[" + std::to_string(i) + "]", shadowMaps[i].cascadeEndClip);
		}
	}

	void CascadedDirectionalShadowMap::removeMap(Shader& shader) {
		shader.removeMap(*this);
	}


	void CascadedDirectionalShadowMap::draw(const SceneCamera* const camera, const RenderScene& scene,
		ShadowmapRepository& repository) {

		//TODO: Develop backup strategy for when scene camera is not available
		if(camera != nullptr)
			computeLightTransforms(*camera);

		FrameBuffer::bind(fbo.token);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		const RenderShader& shader = repository.getSimple2DShader();
		unsigned int hWidth = width / 2;
		unsigned int hHeight = height / 2;
		for (unsigned int i = 0; i < MAPCOUNT; i++) {
			int x = i % 2;
			int y = i / 2;

			Viewport::set(x * hWidth, y * hHeight, hWidth, hHeight);
			shader.bind<glm::mat4>("lightTransform", shadowMaps[i].lightTransform);
			scene.drawStaticObjects(shader); //Note: currently only static objects
		}

		FrameBuffer::unbind();
	}


	void CascadedDirectionalShadowMap::computeLightTransforms(const SceneCamera& camera) {

		glm::mat4 lightView = glm::lookAt(camera.transform.getPosition(), 
			camera.transform.getPosition() - light.transform.getForwardDirection(),
			vec3(0.f, 1.f, 0.f));

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

			glm::mat4&& lightProj = glm::ortho(-maxX, -minX, -maxY, -minY, minZ, maxZ);
			shadowMaps[i].lightTransform = lightProj * lightView;

			near = far;
		}
	}


	TextureType CascadedDirectionalShadowMap::getTextureType() const {
		return TextureType::Texture2D;
	}



}