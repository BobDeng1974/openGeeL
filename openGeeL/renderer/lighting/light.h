#ifndef LIGHT_H
#define LIGHT_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <functional>
#include <string>
#include "../sceneobject.h"

using glm::vec3;

namespace geeL {

	class ShadowMap;
	class Camera;
	class Transform;
	class RenderScene;
	class Shader;
	const RenderScene;


	class Light : public SceneObject {

	public:
		vec3 diffuse;

		Light(Transform& transform, vec3 diffuse, const std::string& name = "Light");
		~Light();

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, const std::string& name) const;
		virtual void forwardBind(const Shader& shader, std::string name, const std::string& transformName) const;

		const ShadowMap* const getShadowMap() const;
		void setShadowMap(ShadowMap& map);

		virtual void addShadowmap(Shader& shader, const std::string& name);

		virtual void renderShadowmap(const Camera& camera, 
			std::function<void(const Shader&)> renderCall, const Shader& shader);

		//Computes experienced intensity at given point. Ranges between 0 and 1
		virtual float getIntensity(glm::vec3 point) const;


	protected:
		unsigned int shadowmapFBO;
		unsigned int shadowmapID;

		ShadowMap* shadowMap;

	};
}

#endif
