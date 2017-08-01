#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "texturing/rendertexturecube.h"
#include "shader/rendershader.h"
#include "framebuffer/framebuffer.h"
#include "framebuffer/cubebuffer.h"
#include "primitives/screencube.h"
#include "irrmap.h"

using namespace glm;

namespace geeL {


	IrradianceMap::IrradianceMap(const CubeMap& environmentMap, CubeBuffer& frameBuffer, unsigned int resolution)
		: DynamicCubeMap(new RenderTextureCube(resolution)), environmentMap(environmentMap), frameBuffer(frameBuffer),
			conversionShader(new RenderShader("renderer/cubemapping/envconvert.vert", 
				"renderer/cubemapping/irrmap.frag")) {

		conversionShader->mapOffset = 1;
		conversionShader->addMap(environmentMap.getTexture(), "environmentMap");
	}

	IrradianceMap::~IrradianceMap() {
		delete texture;
		delete conversionShader;
	}

	void IrradianceMap::bindMap(const RenderShader& shader, std::string name) const {
		environmentMap.bindMap(shader, name);
	}

	void IrradianceMap::add(RenderShader& shader, std::string name) const {
		shader.addMap(*texture, name + "irradiance");
	}

	void IrradianceMap::draw() {
		frameBuffer.init(*texture);

		glm::mat4 projection = perspective(90.0f, 1.0f, 0.1f, 10.0f);
		glm::mat4 views[] = {
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f,  0.0f,  0.0f),  vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  1.0f,  0.0f),  vec3(0.0f,  0.0f,  1.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f,  0.0f),  vec3(0.0f,  0.0f, -1.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f,  1.0f),  vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f, -1.0f),  vec3(0.0f, -1.0f,  0.0f))
		};

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		conversionShader->bind<glm::mat4>("projection", projection);
		conversionShader->loadMaps();

		frameBuffer.fill([&](unsigned int side) {
			conversionShader->bind<glm::mat4>("view", views[side]);
			SCREENCUBE.drawComplete();
		});

	}

}