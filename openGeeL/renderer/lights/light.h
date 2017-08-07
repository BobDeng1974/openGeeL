#ifndef LIGHT_H
#define LIGHT_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <functional>
#include <string>
#include "sceneobject.h"

using glm::vec3;

namespace geeL {

	class Camera;
	class ShadowMap;
	class SceneCamera;
	class Transform;
	class RenderScene;
	class RenderShader;
	class SceneShader;
	const RenderScene;

	enum class ShaderTransformSpace;
	

	class Light : public SceneObject {

	public:
		Light(Transform& transform, vec3 diffuse, const std::string& name = "Light");
		~Light();

		virtual void bind(const RenderShader& shader, 
			const std::string& name, ShaderTransformSpace space, const Camera* const camera = nullptr) const;

		virtual void bind(const SceneShader& shader,
			const std::string& name, const Camera* const camera = nullptr) const;

		const ShadowMap* const getShadowMap() const;
		ShadowMap* const getShadowMap();
		void setShadowMap(ShadowMap& map);

		//Add shadow map to given shader
		void addShadowmap(RenderShader& shader, const std::string& name);

		//Remove shadow map from given shader
		void removeShadowmap(RenderShader& shader);

		void renderShadowmap(const SceneCamera* const camera, 
			std::function<void(const RenderShader&)> renderCall, const RenderShader& shader);

		//Draw shadow map no matter the lights properties. E.g. if it is static or not
		void renderShadowmapForced(const SceneCamera* const camera,
			std::function<void(const RenderShader&)> renderCall, const RenderShader& shader);

		//Computes experienced intensity at given point. Ranges between 0 and 1
		virtual float getIntensity(glm::vec3 point) const;

		//Returns diffuse color of this light
		vec3 getColor() const;
		void setColor(vec3 color);

	protected:
		vec3 diffuse;

		unsigned int shadowmapFBO;
		unsigned int shadowmapID;

		ShadowMap* shadowMap;

	};
}

#endif
