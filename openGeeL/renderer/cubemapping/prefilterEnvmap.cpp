#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "texturing/texture.h"
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
			: DynamicCubeMap(std::unique_ptr<TextureCube>(
				new TextureCube(resolution,
					ColorType::RGB16,
					FilterMode::Trilinear,
					WrapMode::ClampEdge)))
			, environmentMap(environmentMap)
			, frameBuffer(frameBuffer)
			, conversionShader(new RenderShader("shaders/cubemapping/envconvert.vert",
				"shaders/cubemapping/prefilterEnvmap.frag")) {

		getTexture().mipmap();

		conversionShader->setMapOffset(1);
		conversionShader->addMap(environmentMap.getTexture(), "environmentMap");
	}

	PrefilteredEnvironmentMap::~PrefilteredEnvironmentMap() {
		delete conversionShader;
	}


	void PrefilteredEnvironmentMap::add(RenderShader& shader, std::string name) const {
		shader.addMap(getTexture(), name + "prefilterEnv");
	}


	void PrefilteredEnvironmentMap::draw() {
		frameBuffer.init(getTextureCube());

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