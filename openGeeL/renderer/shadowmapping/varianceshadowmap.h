#ifndef VARIANCESHADOWMAP_H
#define VARIANCESHADOWMAP_H

#include "framebuffer/stackbuffer.h"
#include "texturing/rendertexture.h"
#include "postprocessing/gaussianblur.h"
#include "simpleshadowmap.h"

namespace geeL {

	//Implementation of variance shadow maps according to
	//'http://www.punkuser.net/vsm/vsm_paper.pdf'
	class VarianceSpotLightMap : public Shadowmap {

	public:
		VarianceSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config);

		virtual void bind(unsigned int layer) const;
		virtual void bindImage(unsigned int position = 0, AccessType access = AccessType::All) const;

		virtual void draw(const SceneCamera* const camera, const RenderScene& scene,
			ShadowmapRepository& repository);

		virtual void bindData(const Shader& shader, const std::string& name);

		virtual Resolution getScreenResolution() const;

	protected:
		virtual void computeLightTransform();

	private:
		float shadowBias, farPlane;
		unsigned int resolution;
		glm::mat4 lightTransform;
		const SpotLight& spotLight;

		StackBuffer blurBuffer;
		RenderTexture blurTexture;
		GaussianBlur blur;

	};

}

#endif
