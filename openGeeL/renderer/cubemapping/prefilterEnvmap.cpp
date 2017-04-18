#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "../shader/shader.h"
#include "../utility/framebuffer.h"
#include "../primitives/screencube.h"
#include "prefilterEnvmap.h"

#include <iostream>

using namespace glm;

namespace geeL {

	PrefilteredEnvironmentMap::PrefilteredEnvironmentMap(const CubeMap& environmentMap, unsigned int resolution) 
		: environmentMap(environmentMap), conversionShader(new Shader("renderer/cubemapping/envconvert.vert",
			"renderer/cubemapping/prefilterEnvmap.frag")), resolution(resolution) {

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		for (unsigned int side = 0; side < 6; side++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, GL_RGB16F,
				resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glGenFramebuffers(1, &fbo);
		glGenRenderbuffers(1, &rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		convertEnvironmentMap();
	}

	PrefilteredEnvironmentMap::~PrefilteredEnvironmentMap() {
		delete conversionShader;
	}


	void PrefilteredEnvironmentMap::add(Shader& shader, std::string name) const {
		shader.addMap(id, name + "prefilterEnv", GL_TEXTURE_CUBE_MAP);
	}


	void PrefilteredEnvironmentMap::convertEnvironmentMap() {

		glm::mat4 projection = perspective(90.0f, 1.0f, 0.1f, 10.0f);
		glm::mat4 views[] = {
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f,  0.0f,  0.0f),  vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  1.0f,  0.0f),  vec3(0.0f,  0.0f,  1.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f,  0.0f),  vec3(0.0f,  0.0f, -1.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f,  1.0f),  vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f, -1.0f),  vec3(0.0f, -1.0f,  0.0f))
		};

		conversionShader->use();
		conversionShader->setMat4("projection", projection);

		conversionShader->setInteger("environmentMap", conversionShader->mapOffset);
		std::list<unsigned int> maps = { environmentMap.getID() };
		conversionShader->loadMaps(maps, GL_TEXTURE_CUBE_MAP);

		FrameBuffer::bind(fbo);
		unsigned int mipLevels = 5;
		//Generate mipmaps according to roughness strength
		for (unsigned int mip = 0; mip < mipLevels; mip++) {

			float roughness = (float)mip / (float)(mipLevels - 1);
			conversionShader->setFloat("roughness", roughness);

			unsigned int mipResolution = resolution * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipResolution, mipResolution);
			glViewport(0, 0, mipResolution, mipResolution);

			for (unsigned int side = 0; side < 6; side++) {
				conversionShader->setMat4("view", views[side]);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, id, mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				SCREENCUBE.drawComplete();
			}
		}
 
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		FrameBuffer::unbind();
		FrameBuffer::remove(fbo);
	}

}