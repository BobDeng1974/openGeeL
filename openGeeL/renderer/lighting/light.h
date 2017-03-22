#ifndef LIGHT_H
#define LIGHT_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <functional>
#include <string>
#include "../sceneobject.h"

using glm::vec3;

namespace geeL {

	class Camera;
	class Transform;
	class RenderScene;
	class Shader;
	const RenderScene;

	enum class ShadowmapResolution {
		Adaptive = 0,
		Small = 128,
		Medium = 256,
		High = 512,
		VeryHigh = 1024
	};


	class Light : public SceneObject {

	public:
		vec3 diffuse;

		Light(Transform& transform, vec3 diffuse, float shadowBias, const std::string& name = "Light");

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, const std::string& name) const;
		virtual void forwardBind(const Shader& shader, std::string name, const std::string& transformName) const;

		virtual void initShadowmap();
		virtual void addShadowmap(Shader& shader, const std::string& name);

		//Render function for shadow maps. Takes current scene camera, render function
		//that  draws desired objects of scene and the actual shadow map shader
		virtual void renderShadowmap(const Camera& camera, 
			std::function<void(const Shader&)> renderCall, const Shader& shader);

		virtual void computeLightTransform() = 0;

		//Computes experienced intensity at given point. Ranges between 0 and 1
		virtual float getIntensity(glm::vec3 point) const;

		float getShadowBias() const;
		void setShadowBias(float bias);

		void setResolution(ShadowmapResolution resolution);
		const int getShadowMapID() const;
		const int getShadowMapFBO() const;
		const glm::mat4& getLightTransform() const;

	protected:
		unsigned int shadowmapFBO;
		unsigned int shadowmapID;
		ShadowmapResolution resolution;
		int shadowmapHeight;
		int shadowmapWidth;
		float shadowBias, dynamicBias;
		glm::mat4 lightTransform;

		//Set width and height of shadow map without
		//affecting resolution
		void setDimensions(int resolution);

		//Dynamically change shadow map resolution
		virtual void adaptShadowmap(const Camera& camera);

		//Dynamically change resolution of shadow map 
		//depending on distance to render camera. Returns true if 
		//resolution has changed
		virtual bool adaptShadowmapResolution(float distance) = 0;

		//Bind resolution to shadow map texture(s)
		virtual void bindShadowmapResolution() const;

	};
}

#endif
