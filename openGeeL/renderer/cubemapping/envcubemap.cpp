#define GLEW_STATIC
#include <glew.h>
#include <list>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "../shader/shader.h"
#include "../texturing/envmap.h"
#include "../utility/framebuffer.h"
#include "../primitives/screencube.h"
#include "envcubemap.h"

using namespace glm;

namespace geeL {

	EnvironmentCubeMap::EnvironmentCubeMap(const EnvironmentMap& map, unsigned int resolution)
		: map(map), resolution(resolution), conversionShader(new Shader("renderer/cubemapping/envconvert.vert", 
			"renderer/cubemapping/envconvert.frag")) {

		//Init cubemap without textures
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

		unsigned int captureRBO;
		glGenFramebuffers(1, &fbo);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		convertEnvironmentMap();

		//Mip map rendered environment map
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	EnvironmentCubeMap::~EnvironmentCubeMap() {
		delete conversionShader;
	}


	void EnvironmentCubeMap::convertEnvironmentMap() {
		
		glm::mat4 projection = perspective(90.0f, 1.0f, 0.1f, 10.0f);
		glm::mat4 views[] = {
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f,  0.0f,  0.0f),  vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  1.0f,  0.0f),  vec3(0.0f,  0.0f,  1.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f,  0.0f),  vec3(0.0f,  0.0f, -1.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f,  1.0f),  vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f, -1.0f),  vec3(0.0f, -1.0f,  0.0f))
		};

		conversionShader->use();
		conversionShader->setMat4("projection", projection);
		conversionShader->setInteger("environmentMap", conversionShader->mapOffset);

		std::list<unsigned int> maps = { map.getID() };
		conversionShader->loadMaps(maps);

		glViewport(0, 0, resolution, resolution);

		FrameBuffer::bind(fbo);
		for (unsigned int side = 0; side < 6; side++) {
			conversionShader->setMat4("view", views[side]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, id, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			SCREENCUBE.drawComplete();
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		FrameBuffer::unbind();
		FrameBuffer::remove(fbo);
	}

}