#ifndef SIMPLESHADOWMAP_H
#define SIMPLESHADOWMAP_H

#include "shadowmap.h"


namespace geeL {

	struct ScreenInfo;

	class SpotLight;
	class PointLight;
	class DirectionalLight;


	class SimpleShadowMap : public ShadowMap {

	public:
		SimpleShadowMap(const Light& light, float farPlane);

		virtual void init();

		virtual void bindData(const Shader& shader, const std::string& name) = 0;
		virtual void bindMap(Shader& shader, const std::string& name);

		virtual void draw(const Camera& camera,
			std::function<void(const Shader&)> renderCall, const Shader& shader) = 0;

		void setResolution(ShadowmapResolution resolution);


	protected:
		float shadowBias, dynamicBias, farPlane;
		unsigned int id, fbo, width, height;
		ShadowmapResolution resolution;

		//Dynamically change shadow map resolution
		virtual void adaptShadowmap(const Camera& camera);

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
		SimpleSpotLightMap(const SpotLight& light, float farPlane);

		virtual void bindData(const Shader& shader, const std::string& name);

		virtual void draw(const Camera& camera,
			std::function<void(const Shader&)> renderCall, const Shader& shader);

	private:
		const SpotLight& spotLight;
		glm::mat4 lightTransform;

		void computeLightTransform();
		virtual bool adaptShadowmapResolution(float distance);
	};


	class SimplePointLightMap : public SimpleShadowMap {

	public:
		SimplePointLightMap(const PointLight& light, float farPlane);

		virtual void bindData(const Shader& shader, const std::string& name);

		virtual void draw(const Camera& camera,
			std::function<void(const Shader&)> renderCall, const Shader& shader);

	private:
		const PointLight& pointLight;
		std::vector<glm::mat4> lightTransforms;

		void computeLightTransform();
		virtual bool adaptShadowmapResolution(float distance);
	};


	class SimpleDirectionalLightMap : public SimpleShadowMap {

	public:
		SimpleDirectionalLightMap(const DirectionalLight& light, float farPlane);

		virtual void bindData(const Shader& shader, const std::string& name);

		virtual void draw(const Camera& camera,
			std::function<void(const Shader&)> renderCall, const Shader& shader);

	private:
		const DirectionalLight& directionalLight;
		glm::mat4 lightTransform;

		void computeLightTransform(const ScreenInfo& info, glm::vec3 offset);
		virtual bool adaptShadowmapResolution(float distance);

	};


}

#endif
