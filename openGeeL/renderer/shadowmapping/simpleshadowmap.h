#ifndef SIMPLESHADOWMAP_H
#define SIMPLESHADOWMAP_H

#include <vector>
#include "shadowmapconfig.h"

#include "shadowmap.h"


namespace geeL {

	class SpotLight;
	class PointLight;
	class DirectionalLight;


	class SimpleShadowMap : public ShadowMap {

	public:
		SimpleShadowMap(const Light& light, 
			std::unique_ptr<Texture> innerTexture, 
			const ShadowMapConfiguration& config);

		virtual void draw(const SceneCamera* const camera, const RenderScene& scene,
			ShadowmapRepository& repository);

		virtual void bindData(const Shader& shader, const std::string& name);
		virtual ShadowMapType getType() const;

		void setResolution(ShadowmapResolution resolution);
		virtual Resolution getScreenResolution() const;

		float getShadowBias() const;
		void setShadowBias(float bias);

		int getSoftShadowResolution() const;
		void setSoftShadowResolution(unsigned int resolution);

		float getSoftShadowScale() const;
		void setSoftShadowScale(float scale);

	protected:
		const ShadowMapType type;
		float shadowBias, farPlane, softShadowScale;
		unsigned int resolution, softShadowResolution, depthID;

		SimpleShadowMap(const SimpleShadowMap& other) = delete;
		SimpleShadowMap& operator= (const SimpleShadowMap& other) = delete;

		virtual void drawMap(const RenderScene& scene, ShadowmapRepository& repository) = 0;
		virtual void computeLightTransform() = 0;

	};



	class SimpleSpotLightMap : public SimpleShadowMap {

	public:
		SimpleSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config);

		virtual void bindData(const Shader& shader, const std::string& name);

	protected:
		const SpotLight& spotLight;
		glm::mat4 lightTransform;

		SimpleSpotLightMap(const SpotLight& light, const ShadowMapConfiguration& config, bool init);

		virtual void drawMap(const RenderScene& scene, ShadowmapRepository& repository);
		virtual void computeLightTransform();

	};



	class SimplePointLightMap : public SimpleShadowMap {

	public:
		SimplePointLightMap(const PointLight& light, const ShadowMapConfiguration& config);
		
		virtual void bindData(const Shader& shader, const std::string& name);

	private:
		const PointLight& pointLight;
		std::vector<glm::mat4> lightTransforms;

		virtual void drawMap(const RenderScene& scene, ShadowmapRepository& repository);
		virtual void computeLightTransform();
		
	};



	class SimpleDirectionalLightMap : public SimpleShadowMap {

	public:
		SimpleDirectionalLightMap(const DirectionalLight& light, const ShadowMapConfiguration& config);

		virtual void bindData(const Shader& shader, const std::string& name);

	private:
		const DirectionalLight& directionalLight;
		glm::mat4 lightTransform;

		virtual void drawMap(const RenderScene& scene, ShadowmapRepository& repository);
		virtual void computeLightTransform();

	};

}

#endif
