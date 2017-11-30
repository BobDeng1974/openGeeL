#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "texturing/rendertexturecube.h"
#include "shader/rendershader.h"
#include "framebuffer/framebuffer.h"
#include "framebuffer/cubebuffer.h"
#include "primitives/screencube.h"
#include "prefilterEnvmap.h"

using namespace glm;

namespace geeL {

	PrefilteredEnvironmentMap::PrefilteredEnvironmentMap(const CubeMap& environmentMap, 
		CubeBuffer& frameBuffer, 
		unsigned int resolution)
			: DynamicCubeMap(new RenderTextureCube(resolution, 
				ColorType::RGB16, 
				WrapMode::ClampEdge, 
				FilterMode::Trilinear))
			, environmentMap(environmentMap)
			, frameBuffer(frameBuffer)
			, conversionShader(new RenderShader("shaders/cubemapping/envconvert.vert",
				"shaders/cubemapping/prefilterEnvmap.frag")) {

		texture->mipmap();

		conversionShader->mapOffset = 1;
		conversionShader->addMap(environmentMap.getTexture(), "environmentMap");
	}

	PrefilteredEnvironmentMap::~PrefilteredEnvironmentMap() {
		delete conversionShader;
	}


	void PrefilteredEnvironmentMap::add(RenderShader& shader, std::string name) const {
		shader.addMap(*texture, name + "prefilterEnv");
	}


	void PrefilteredEnvironmentMap::draw() {
		frameBuffer.init(*texture);

		glm::mat4 projection = perspective(90.0f, 1.0f, 0.1f, 10.0f);
		glm::mat4 views[] = {
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f,  0.0f,  0.0f),  vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  1.0f,  0.0f),  vec3(0.0f,  0.0f,  1.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f,  0.0f),  vec3(0.0f,  0.0f, -1.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f,  1.0f),  vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f, -1.0f),  vec3(0.0f, -1.0f,  0.0f))
		};

		conversionShader->bind<glm::mat4>("projection", projection);
		conversionShader->loadMaps();

		unsigned int mipLevels = 5;
		//Generate mipmaps according to roughness strength
		for (unsigned int mip = 0; mip < mipLevels; mip++) {
			float roughness = (float)mip / (float)(mipLevels - 1);
			conversionShader->bind<float>("roughness", roughness);
			
			frameBuffer.resize(std::powf(0.5f, mip));
			frameBuffer.fill([&](unsigned int side) {
				conversionShader->bind<glm::mat4>("view", views[side]);
				SCREENCUBE.drawComplete();
			}, mip);
		}

	}

}