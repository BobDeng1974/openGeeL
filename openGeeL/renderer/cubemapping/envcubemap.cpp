#define GLEW_STATIC
#include <glew.h>
#include <list>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "texturing/rendertexturecube.h"
#include "shader/rendershader.h"
#include "texturing/envmap.h"
#include "framebuffer/framebuffer.h"
#include "framebuffer/cubebuffer.h"
#include "primitives/screencube.h"
#include "envcubemap.h"

using namespace glm;

namespace geeL {

	EnvironmentCubeMap::EnvironmentCubeMap(const EnvironmentMap& map, 
		CubeBuffer& frameBuffer, 
		unsigned int resolution)
			: CubeMap(new RenderTextureCube(resolution, 
				ColorType::RGB16, 
				WrapMode::ClampEdge, 
				FilterMode::Trilinear)) {

		draw(map, frameBuffer);
	}


	void EnvironmentCubeMap::draw(const EnvironmentMap& map, CubeBuffer& frameBuffer) {
		RenderShader conversionShader("shaders/cubemapping/envconvert.vert",
			"shaders/cubemapping/envconvert.frag");

		conversionShader.mapOffset = 1;
		conversionShader.addMap(map, "environmentMap");

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

		conversionShader.bind<glm::mat4>("projection", projection);
		conversionShader.loadMaps();

		frameBuffer.fill([&](unsigned int side) {
			conversionShader.bind<glm::mat4>("view", views[side]);
			SCREENCUBE.drawComplete();
		});

		texture->mipmap();
	}

}