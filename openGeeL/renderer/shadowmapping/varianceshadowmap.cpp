#define GLEW_STATIC
#include <glew.h>
#include "primitives/screenquad.h"
#include "glwrapper/glguards.h"
#include "varianceshadowmap.h"

namespace geeL {

	VarianceSpotLightMap::VarianceSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config)
		: SimpleSpotLightMap(light, config, false), temp(*this, Resolution((int)config.resolution)), blur(KernelSize::Depth, 1.5f), 
			texture(Resolution((int)config.resolution), ColorType::RGB16, WrapMode::ClampEdge, FilterMode::Linear) {

		init();

		blurBuffer.initResolution(Resolution((int)config.resolution));
		blur.setImage(temp);
		blur.init(PostProcessingParameter(ScreenQuad::get(), blurBuffer, Resolution((int)config.resolution)));
	}



	void VarianceSpotLightMap::init() {

		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, resolution, resolution, 0, GL_RG, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);


	}

	void VarianceSpotLightMap::draw(const SceneCamera* const camera, const RenderScene& scene,
		ShadowmapRepository& repository) {

		computeLightTransform();

		buffer.add(getInnerTexture());
		buffer.fill([this, &scene, &repository]() {
			this->drawMap(scene, repository);


		});
		

		const RenderShader& shader = repository.getVariance2DShader();
		//SimpleSpotLightMap::draw(camera, scene, shader); //Note: currently only static objects

		//Blur shadow map
		DepthGuard depthGuard(true);

		glCullFace(GL_FRONT);
		blurBuffer.push(texture);
		blur.bindValues();
		blurBuffer.fill(blur);
		glCullFace(GL_BACK);

	}



	unsigned int VarianceSpotLightMap::getID() const {
		return texture.getID();
	}
}