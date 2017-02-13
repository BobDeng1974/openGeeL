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

	class Light : public SceneObject {

	public:
		vec3 diffuse;
		vec3 specular;

		Light(Transform& transform, vec3 diffuse, vec3 specular, float shadowBias) 
			: SceneObject(transform), diffuse(diffuse), specular(specular), shadowBias(shadowBias) {}

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, int index, std::string name) const;
		virtual void forwardBind(const Shader& shader, int index, std::string name) const;

		virtual void initShadowmap();
		virtual void addShadowmap(Shader& shader, std::string name);
		virtual void renderShadowmap(const RenderScene& scene, const Shader& shader);
		virtual void computeLightTransform() = 0;

		const int getShadowMapID() const;
		const int getShadowMapFBO() const;

	protected:
		unsigned int shadowmapFBO;
		unsigned int shadowmapID;
		int shadowmapHeight;
		int shadowmapWidth;
		float shadowBias;
		glm::mat4 lightTransform;

	};
}

#endif
