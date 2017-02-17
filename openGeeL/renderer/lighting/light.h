#ifndef LIGHT_H
#define LIGHT_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <string>
#include "../sceneobject.h"

using glm::vec3;

namespace geeL {

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
		vec3 specular;

		Light(Transform& transform, vec3 diffuse, vec3 specular, float shadowBias);

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, int index, std::string name) const;
		virtual void forwardBind(const Shader& shader, int index, std::string name) const;

		virtual void initShadowmap();
		virtual void addShadowmap(Shader& shader, std::string name);
		virtual void renderShadowmap(const RenderScene& scene, const Shader& shader);
		virtual void computeLightTransform() = 0;

		//Computes experienced intensity at given point. Ranges between 0 and 1
		virtual float getIntensity(glm::vec3 point) const;

		void setResolution(ShadowmapResolution resolution);
		const int getShadowMapID() const;
		const int getShadowMapFBO() const;

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
		void adaptShadowmap(const RenderScene& scene);

		//Dynamically change resolution of shadow map 
		//depending on distance to render camera. Returns true if 
		//resolution has changed
		virtual bool adaptShadowmapResolution(float distance) = 0;

		//Bind resolution to shadow map texture(s)
		virtual void bindShadowmapResolution() const;

	};
}

#endif
