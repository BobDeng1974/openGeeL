#define GLEW_STATIC
#include <glew.h>
#include "shader/computeshader.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "depthreader.h"

namespace geeL {
	
	DepthReader::DepthReader(TextureProvider& provider) 
		: provider(provider)
		, depth(0.f) {

		shader = new ComputeShader("shaders/nearest.com.glsl");

		const ITexture& positionTexture = provider.requestPosition();
		shader->addMap(positionTexture, "gPosition");

		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 4, NULL, GL_STATIC_READ);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	DepthReader::~DepthReader() {
		delete shader;
	}


	void DepthReader::readDepth() {
		shader->loadMaps();
		shader->bindParameters();

		shader->invoke(1, 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4, &depth);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	float DepthReader::getDepth() const {
		return depth;
	}

}