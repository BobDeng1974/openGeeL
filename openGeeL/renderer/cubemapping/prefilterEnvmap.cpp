#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "../shader/rendershader.h"
#include "../framebuffer/framebuffer.h"
#include "../framebuffer/cubebuffer.h"
#include "../primitives/screencube.h"
#include "prefilterEnvmap.h"

#include <iostream>

using namespace glm;

namespace geeL {

	PrefilteredEnvironmentMap::PrefilteredEnvironmentMap(const CubeMap& environmentMap, CubeBuffer& frameBuffer, unsigned int resolution)
		: environmentMap(environmentMap), frameBuffer(frameBuffer), conversionShader(new RenderShader("renderer/cubemapping/envconvert.vert",
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
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		Texture::mipmapCube(id);
		conversionShader->mapOffset = 1;
	}

	PrefilteredEnvironmentMap::~PrefilteredEnvironmentMap() {
		delete conversionShader;
	}


	void PrefilteredEnvironmentMap::add(RenderShader& shader, std::string name) const {
		shader.addMap(id, name + "prefilterEnv", GL_TEXTURE_CUBE_MAP);
	}


	void PrefilteredEnvironmentMap::update() {
		frameBuffer.init(resolution, id);

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

		unsigned int mipLevels = 5;
		//Generate mipmaps according to roughness strength
		for (unsigned int mip = 0; mip < mipLevels; mip++) {
			float roughness = (float)mip / (float)(mipLevels - 1);
			conversionShader->setFloat("roughness", roughness);
			unsigned int mipResolution = resolution * std::pow(0.5f, mip);
			
			frameBuffer.resize(mipResolution, mipResolution);
			frameBuffer.fill([&](unsigned int side) {
				conversionShader->setMat4("view", views[side]);
				SCREENCUBE.drawComplete();
			}, mip);

		}
	}

}