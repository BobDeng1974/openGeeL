#ifndef VARIANCESHADOWMAP_H
#define VARIANCESHADOWMAP_H

#include "framebuffer/stackbuffer.h"
#include "texturing/rendertexture.h"
#include "postprocessing/gaussianblur.h"
#include "simpleshadowmap.h"

namespace geeL {

	//Implementation of variance shadow maps according to
	//'http://www.punkuser.net/vsm/vsm_paper.pdf'
	class VarianceSpotLightMap : public SimpleSpotLightMap {

	public:
		VarianceSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config);

		virtual void draw(const SceneCamera* const camera, std::function<void(const RenderShader&)> renderCall,
			const ShadowmapRepository& repository);

		virtual unsigned int getID() const;

	protected:
		virtual void init();
		virtual void bindShadowmapResolution() const;

	private:
		StackBuffer buffer;
		RenderTexture texture;
		RenderTexture temp;
		GaussianBlur blur;

	};

}

#endif