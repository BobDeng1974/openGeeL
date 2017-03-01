#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "../shader/shader.h"
#include "../utility/framebuffer.h"
#include "../primitives/screencube.h"
#include "irrmap.h"

using namespace glm;

namespace geeL {


	IrradianceMap::IrradianceMap(const CubeMap& environmentMap, unsigned int resolution)
		: environmentMap(environmentMap), conversionShader(new Shader("renderer/cubemapping/envconvert.vert", 
			"renderer/cubemapping/irrmap.frag")), resolution(resolution) {

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		for (unsigned int side = 0; side < 6; side++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, GL_RGB16F, 
				resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		//TODO: maybe use framebuffer object here
		unsigned int captureRBO;
		glGenFramebuffers(1, &fbo);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		convertEnvironmentMap();
	}

	IrradianceMap::~IrradianceMap() {
		delete conversionShader;
	}


	void IrradianceMap::convertEnvironmentMap() {

		glm::mat4 projection = perspective(radians(90.0f), 1.0f, 0.1f, 10.0f);
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

		std::list<unsigned int> maps = { environmentMap.getID() };
		conversionShader->loadMaps(maps, GL_TEXTURE_CUBE_MAP);
		
		glViewport(0, 0, resolution, resolution);

		FrameBuffer::bind(fbo);
		for (unsigned int side = 0; side < 6; side++) {
			conversionShader->setMat4("view", views[side]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, id, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			SCREENCUBE.drawComplete();
		}

		FrameBuffer::unbind();
	}

	void IrradianceMap::bind(const Shader& shader, std::string name) const {

		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(shader.program, name.c_str()), 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	}


}