#define GLEW_STATIC
#include <glew.h>
#include "primitives/screenquad.h"
#include "utility/glguards.h"
#include "varianceshadowmap.h"

namespace geeL {

	VarianceSpotLightMap::VarianceSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config)
		: SimpleSpotLightMap(light, config, false), temp(*this, Resolution((int)config.resolution)), blur(KernelSize::Depth, 1.5f), 
			texture(Resolution((int)config.resolution), ColorType::RGB16, WrapMode::ClampEdge, FilterMode::Linear) {

		init();

		buffer.initResolution(Resolution((int)config.resolution));
		blur.setImage(temp);
		blur.init(PostProcessingParameter(ScreenQuad::get(), buffer, Resolution((int)config.resolution)));
	}



	void VarianceSpotLightMap::init() {

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);


		glGenFramebuffers(1, &fbo.token);
		FrameBuffer::bind(fbo.token);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);

		FrameBuffer::unbind();
	}

	void VarianceSpotLightMap::draw(const SceneCamera* const camera, std::function<void(const RenderShader&)> renderCall, 
		ShadowmapRepository& repository) {

		const RenderShader& shader = repository.getVariance2DShader();
		SimpleSpotLightMap::draw(camera, renderCall, shader);

		//Blur shadow map
		DepthGuard depthGuard(true);

		glCullFace(GL_FRONT);
		buffer.push(texture);
		blur.bindValues();
		buffer.fill(blur);
		glCullFace(GL_BACK);

	}



	unsigned int VarianceSpotLightMap::getID() const {
		return texture.getID();
	}

	

	void VarianceSpotLightMap::bindShadowmapResolution() const {
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F,
			width, height, 0, GL_RG, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
	}


}