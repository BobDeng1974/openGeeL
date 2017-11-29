#ifndef VARIANCESHADOWMAP_H
#define VARIANCESHADOWMAP_H

#include "framebuffer/stackbuffer.h"
#include "texturing/rendertexture.h"
#include "postprocessing/gaussianblur.h"
#include "simpleshadowmap.h"

namespace geeL {

	//Implementation of variance shadow maps according to
	//'http://www.punkuser.net/vsm/vsm_paper.pdf'
	class VarianceSpotLightMap : public ShadowMap {

	public:
		VarianceSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config);

		virtual void draw(const SceneCamera* const camera, const RenderScene& scene,
			ShadowmapRepository& repository);

		virtual void bindData(const Shader& shader, const std::string& name);

		virtual Resolution getScreenResolution() const;
		virtual unsigned int getID() const;

	protected:
		virtual void computeLightTransform();

	private:
		float shadowBias, farPlane;
		unsigned int resolution;
		glm::mat4 lightTransform;
		const SpotLight& spotLight;

		StackBuffer blurBuffer;
		RenderTexture blurTexture;
		RenderTexture temp;
		GaussianBlur blur;

	};

}

#endif
