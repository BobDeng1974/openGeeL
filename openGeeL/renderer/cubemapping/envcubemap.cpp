#define GLEW_STATIC
#include <glew.h>
#include <list>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "../shader/rendershader.h"
#include "../texturing/envmap.h"
#include "../framebuffer/framebuffer.h"
#include "../framebuffer/cubebuffer.h"
#include "../primitives/screencube.h"
#include "envcubemap.h"

using namespace glm;

namespace geeL {

	EnvironmentCubeMap::EnvironmentCubeMap(const EnvironmentMap& map, CubeBuffer& frameBuffer, unsigned int resolution)
		: map(map), frameBuffer(frameBuffer), resolution(resolution), 
			conversionShader(new RenderShader("renderer/cubemapping/envconvert.vert", 
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
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		conversionShader->mapOffset = 1;
		conversionShader->addMap(map.getID(), "environmentMap", TextureType::Texture2D);

		update();
	}

	EnvironmentCubeMap::~EnvironmentCubeMap() {
		delete conversionShader;
	}


	void EnvironmentCubeMap::update() {
		frameBuffer.init(resolution, id);

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
		conversionShader->loadMaps();

		frameBuffer.fill([&](unsigned int side) {
			conversionShader->setMat4("view", views[side]);
			SCREENCUBE.drawComplete();
		});

		//Mip map rendered environment map
		Texture::mipmap(TextureType::TextureCube, id);
	}

}