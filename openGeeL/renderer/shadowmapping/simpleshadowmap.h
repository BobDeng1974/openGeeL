#ifndef SIMPLESHADOWMAP_H
#define SIMPLESHADOWMAP_H

#include <vector>
#include "shadowmapconfig.h"
#include "shadowmap.h"

#include "framebuffer/stackbuffer.h"
#include "texturing/rendertexture.h"
#include "postprocessing/gaussianblur.h"

namespace geeL {

	struct ScreenInfo;

	class SpotLight;
	class PointLight;
	class DirectionalLight;


	class SimpleShadowMap : public ShadowMap {

	public:
		SimpleShadowMap(const Light& light, const ShadowMapConfiguration& config);

		virtual void bindData(const Shader& shader, const std::string& name);
		virtual void removeMap(Shader& shader);


		void setResolution(ShadowmapResolution resolution);

		float getShadowBias() const;
		void setShadowBias(float bias);

		int getSoftShadowResolution() const;
		void setSoftShadowResolution(unsigned int resolution);

		float getSoftShadowScale() const;
		void setSoftShadowScale(float scale);


	protected:
		float shadowBias, dynamicBias, farPlane, softShadowScale;
		unsigned int resolution, softShadowResolution, depthID;

		SimpleShadowMap(const SimpleShadowMap& other) = delete;
		SimpleShadowMap& operator= (const SimpleShadowMap& other) = delete;

		virtual void init();

	};



	class SimpleSpotLightMap : public SimpleShadowMap {

	public:
		SimpleSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config);

		virtual void bindData(const Shader& shader, const std::string& name);

		virtual void draw(const SceneCamera* const camera, const RenderScene& scene,
			ShadowmapRepository& repository);

		void draw(const SceneCamera* const camera, const RenderScene& scene,
			const RenderShader& shader);

		virtual TextureType getTextureType() const;

	protected:
		const SpotLight& spotLight;
		glm::mat4 lightTransform;

		SimpleSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config, bool init);

		void computeLightTransform();

	};



	class SimplePointLightMap : public SimpleShadowMap {

	public:
		SimplePointLightMap(const PointLight& light, const ShadowMapConfiguration& config);
		
		virtual void bindData(const Shader& shader, const std::string& name);

		virtual void draw(const SceneCamera* const camera, const RenderScene& scene,
			ShadowmapRepository& repository);

		virtual TextureType getTextureType() const;

	protected:
		virtual void init();

	private:
		const PointLight& pointLight;
		std::vector<glm::mat4> lightTransforms;

		void computeLightTransform();
		
	};



	class SimpleDirectionalLightMap : public SimpleShadowMap {

	public:
		SimpleDirectionalLightMap(const DirectionalLight& light, const ShadowMapConfiguration& config);

		virtual void bindData(const Shader& shader, const std::string& name);

		virtual void draw(const SceneCamera* const camera, const RenderScene& scene,
			ShadowmapRepository& repository);

		virtual TextureType getTextureType() const;

	private:
		const DirectionalLight& directionalLight;
		glm::mat4 lightTransform;

		void computeLightTransform();

	};


}

#endif
