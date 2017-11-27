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

		virtual void bindData(const Shader& shader, const std::string& name) = 0;
		virtual void removeMap(Shader& shader);

		virtual void draw(const SceneCamera* const camera,
			std::function<void(const RenderShader&)> renderCall, ShadowmapRepository& repository) = 0;

		virtual TextureType getTextureType() const = 0;

		void setResolution(ShadowmapResolution resolution);

		float getShadowBias() const;
		void setShadowBias(float bias);

		int getSoftShadowResolution() const;
		void setSoftShadowResolution(unsigned int resolution);

		float getSoftShadowScale() const;
		void setSoftShadowScale(float scale);


	protected:
		float shadowBias, dynamicBias, farPlane, softShadowScale;
		unsigned int width, height, softShadowResolution, depthID;
		ShadowmapResolution resolution;

		SimpleShadowMap(const SimpleShadowMap& other) = delete;
		SimpleShadowMap& operator= (const SimpleShadowMap& other) = delete;

		virtual void init();

		//Dynamically change shadow map resolution
		virtual void adaptShadowmap(const SceneCamera* const camera);

		//Dynamically change resolution of shadow map 
		//depending on distance to render camera. Returns true if 
		//resolution has changed
		virtual bool adaptShadowmapResolution(float distance) = 0;

		//Bind resolution to shadow map texture(s)
		virtual void bindShadowmapResolution() const;

		bool setResolution(int resolution, float biasFactor);
	};


	class SimpleSpotLightMap : public SimpleShadowMap {

	public:
		SimpleSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config);

		virtual void bindData(const Shader& shader, const std::string& name);

		virtual void draw(const SceneCamera* const camera, std::function<void(const RenderShader&)> renderCall, 
			ShadowmapRepository& repository);

		void draw(const SceneCamera* const camera, std::function<void(const RenderShader&)> renderCall,
			const RenderShader& shader);

		virtual TextureType getTextureType() const;

	protected:
		const SpotLight& spotLight;
		glm::mat4 lightTransform;

		SimpleSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config, bool init);

		void computeLightTransform();
		virtual bool adaptShadowmapResolution(float distance);

	};



	class SimplePointLightMap : public SimpleShadowMap {

	public:
		SimplePointLightMap(const PointLight& light, const ShadowMapConfiguration& config);
		
		virtual void bindData(const Shader& shader, const std::string& name);

		virtual void draw(const SceneCamera* const camera,
			std::function<void(const RenderShader&)> renderCall, ShadowmapRepository& repository);

		virtual TextureType getTextureType() const;

	protected:
		virtual void init();
		virtual void bindShadowmapResolution() const;

	private:
		const PointLight& pointLight;
		std::vector<glm::mat4> lightTransforms;

		
		void computeLightTransform();
		virtual bool adaptShadowmapResolution(float distance);
		
	};



	class SimpleDirectionalLightMap : public SimpleShadowMap {

	public:
		SimpleDirectionalLightMap(const DirectionalLight& light, const ShadowMapConfiguration& config);

		virtual void bindData(const Shader& shader, const std::string& name);

		virtual void draw(const SceneCamera* const camera,
			std::function<void(const RenderShader&)> renderCall, ShadowmapRepository& repository);

		virtual TextureType getTextureType() const;

	private:
		const DirectionalLight& directionalLight;
		glm::mat4 lightTransform;

		void computeLightTransform();
		virtual bool adaptShadowmapResolution(float distance);

	};


}

#endif
