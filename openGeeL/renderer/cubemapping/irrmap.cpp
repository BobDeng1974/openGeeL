#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "../shader/shader.h"
#include "../framebuffer/framebuffer.h"
#include "../framebuffer/cubebuffer.h"
#include "../primitives/screencube.h"
#include "irrmap.h"

using namespace glm;

namespace geeL {


	IrradianceMap::IrradianceMap(const CubeMap& environmentMap, CubeBuffer& frameBuffer, unsigned int resolution)
		: environmentMap(environmentMap), frameBuffer(frameBuffer), 
			conversionShader(new Shader("renderer/cubemapping/envconvert.vert", 
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
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		update();
	}

	IrradianceMap::~IrradianceMap() {
		delete conversionShader;
	}

	void IrradianceMap::bind(const Shader& shader, std::string name) const {
		environmentMap.bind(shader, name);
	}

	void IrradianceMap::add(Shader& shader, std::string name) const {
		shader.addMap(environmentMap.getID(), name + "albedo", GL_TEXTURE_CUBE_MAP);
		shader.addMap(id, name + "irradiance", GL_TEXTURE_CUBE_MAP);
	}

	void IrradianceMap::update() {
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

		conversionShader->setInteger("environmentMap", conversionShader->mapOffset);

		std::list<unsigned int> maps = { environmentMap.getID() };
		conversionShader->loadMaps(maps, GL_TEXTURE_CUBE_MAP);
		
		frameBuffer.fill([&](unsigned int side) {
			conversionShader->setMat4("view", views[side]);
			SCREENCUBE.drawComplete();
		});
	}
}