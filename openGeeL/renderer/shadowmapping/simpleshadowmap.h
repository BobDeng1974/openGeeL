#ifndef SIMPLESHADOWMAP_H
#define SIMPLESHADOWMAP_H

#include <vector>
#include "shadowmapconfig.h"
#include "shadowmap.h"

namespace geeL {

	struct ScreenInfo;
	//struct ShadowMapConfiguration;

	class SpotLight;
	class PointLight;
	class DirectionalLight;


	class SimpleShadowMap : public ShadowMap {

	public:
		SimpleShadowMap(const Light& light, const ShadowMapConfiguration& config);
		~SimpleShadowMap();

		virtual void bindData(const RenderShader& shader, const std::string& name) = 0;
		virtual void removeMap(RenderShader& shader);

		virtual void draw(const SceneCamera* const camera,
			std::function<void(const RenderShader&)> renderCall, const RenderShader& shader) = 0;

		virtual unsigned int getID() const;
		virtual TextureType getTextureType() const = 0;
		virtual void remove();

		void setResolution(ShadowmapResolution resolution);

		float getShadowBias() const;
		void setShadowBias(float bias);

		int getSoftShadowResolution() const;
		void setSoftShadowResolution(unsigned int resolution);

		float getSoftShadowScale() const;
		void setSoftShadowScale(float scale);


	protected:
		float shadowBias, dynamicBias, farPlane, softShadowScale;
		unsigned int id, fbo, width, height, softShadowResolution;
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
		virtual void bindShadowmapResolution(unsigned int width, unsigned int height) const;

		bool setResolution(int resolution, float biasFactor);
	};

	inline unsigned int SimpleShadowMap::getID() const {
		return id;
	}

	class SimpleSpotLightMap : public SimpleShadowMap {

	public:
		SimpleSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config);

		virtual void bindData(const RenderShader& shader, const std::string& name);

		virtual void draw(const SceneCamera* const camera,
			std::function<void(const RenderShader&)> renderCall, const RenderShader& shader);

		virtual TextureType getTextureType() const;

	private:
		const SpotLight& spotLight;
		glm::mat4 lightTransform;

		void computeLightTransform();
		virtual bool adaptShadowmapResolution(float distance);
	};



	class SimplePointLightMap : public SimpleShadowMap {

	public:
		SimplePointLightMap(const PointLight& light, const ShadowMapConfiguration& config);

		virtual void bindData(const RenderShader& shader, const std::string& name);

		virtual void draw(const SceneCamera* const camera,
			std::function<void(const RenderShader&)> renderCall, const RenderShader& shader);

		virtual TextureType getTextureType() const;

	protected:
		virtual void init();

	private:
		const PointLight& pointLight;
		std::vector<glm::mat4> lightTransforms;

		
		void computeLightTransform();
		virtual bool adaptShadowmapResolution(float distance);
		virtual void bindShadowmapResolution(unsigned int width, unsigned int height) const;
	};



	class SimpleDirectionalLightMap : public SimpleShadowMap {

	public:
		SimpleDirectionalLightMap(const DirectionalLight& light, const ShadowMapConfiguration& config);

		virtual void bindData(const RenderShader& shader, const std::string& name);

		virtual void draw(const SceneCamera* const camera,
			std::function<void(const RenderShader&)> renderCall, const RenderShader& shader);

		virtual TextureType getTextureType() const;

	private:
		const DirectionalLight& directionalLight;
		glm::mat4 lightTransform;

		void computeLightTransform();
		virtual bool adaptShadowmapResolution(float distance);

	};


}

#endif
