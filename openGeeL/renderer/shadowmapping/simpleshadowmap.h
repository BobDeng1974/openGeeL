#ifndef SIMPLESHADOWMAP_H
#define SIMPLESHADOWMAP_H

#include <vector>
#include "shadowmapconfig.h"

#include "shadowmap.h"


namespace geeL {

	class SpotLight;
	class PointLight;
	class DirectionalLight;


	//Simple shadow map that uses PCF for soft shadows
	class SimpleShadowmap : public Shadowmap {

	public:
		SimpleShadowmap(const Light& light, 
			std::unique_ptr<Texture> innerTexture, 
			const ShadowMapConfiguration& config);

		virtual void draw(const SceneCamera* const camera, const RenderScene& scene,
			ShadowmapRepository& repository);

		virtual void bindData(const Shader& shader, const std::string& name);
		virtual ShadowMapType getType() const;

		void setResolution(ShadowmapResolution resolution);
		virtual Resolution getScreenResolution() const;

		float getShadowBias() const;
		float getFarPlane() const;
		float getSoftShadowScale() const;
		int getSoftShadowResolution() const;

		void setShadowBias(float bias);
		void setFarPlane(float value);
		void setSoftShadowScale(float scale);
		void setSoftShadowResolution(unsigned int resolution);

	protected:
		const ShadowMapType type;
		float shadowBias, farPlane, softShadowScale;
		unsigned int resolution, softShadowResolution, depthID;

		SimpleShadowmap(const SimpleShadowmap& other) = delete;
		SimpleShadowmap& operator= (const SimpleShadowmap& other) = delete;

		virtual void drawMap(const RenderScene& scene, ShadowmapRepository& repository) = 0;
		virtual void computeLightTransform() = 0;

	};



	class SimpleSpotLightMap : public SimpleShadowmap {

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



	class SimplePointLightMap : public SimpleShadowmap {

	public:
		SimplePointLightMap(const PointLight& light, const ShadowMapConfiguration& config);
		
		virtual void bindData(const Shader& shader, const std::string& name);

	private:
		const PointLight& pointLight;
		std::vector<glm::mat4> lightTransforms;

		virtual void drawMap(const RenderScene& scene, ShadowmapRepository& repository);
		virtual void computeLightTransform();
		
	};



	class SimpleDirectionalLightMap : public SimpleShadowmap {

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
