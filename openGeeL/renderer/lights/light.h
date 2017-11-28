#ifndef LIGHT_H
#define LIGHT_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <functional>
#include <list>
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
	class Shader;
	class ShadowmapRepository;

	enum class ShaderTransformSpace;
	enum class LightType {
		Directional,
		Point,
		Spot
	};


	//Base class for all light scene objects
	class Light : public SceneObject {

	public:
		Light(Transform& transform, vec3 diffuse, const std::string& name = "Light");
		virtual ~Light();

		virtual void bind(const Shader& shader, 
			const std::string& name, ShaderTransformSpace space, const Camera* const camera = nullptr) const;

		virtual void bind(const SceneShader& shader,
			const std::string& name, const Camera* const camera = nullptr) const;

		const ShadowMap* const getShadowMap() const;
		ShadowMap* getShadowMap();
		void setShadowMap(ShadowMap& map);

		//Add shadow map to given shader
		virtual void addShadowmap(Shader& shader, const std::string& name);

		//Remove shadow map from given shader
		void removeShadowmap(Shader& shader);

		template<typename ...ShadowArgs>
		void renderShadowmap(ShadowArgs&& ...args);

		//Draw shadow map no matter the lights properties. E.g. if it is static or not
		template<typename ...ShadowArgs>
		void renderShadowmapForced(ShadowArgs&& ...args);

		//Computes experienced attenuation at given point. Ranges between 0 and 1
		virtual float getAttenuation(glm::vec3 point) const;

		
		//Sets diffuse color of this light
		void setDiffuse(vec3 diffuse);
		vec3 getDiffuse() const;
		
		//Sets intensity of diffuse color
		void  setIntensity(float value);
		float getIntensity() const;
		

		//Sets base color of diffuse color (Normalized without intensity)
		void setColor(vec3 color);
		vec3 getColor() const;
		

		//Add change callback to this light and specify whether function should
		//be invoked once immediately or not
		void addChangeListener(std::function<void(Light&)> function, bool invoke = false);

		virtual LightType getLightType() const = 0;

	protected:
		vec3 diffuse;
		ShadowMap* shadowMap;

		void onChange();

	private:
		float intensity;
		vec3 color;
		std::list<std::function<void(Light&)>> changeListeners;

		void updateDiffuse();
		void setColorAndIntensityFromDiffuse();
		void setDiffuseOnly(vec3 diffuse);

	};


	template<typename ...ShadowArgs>
	inline void Light::renderShadowmap(ShadowArgs&& ...args) {

		if (shadowMap != nullptr && !transform.isStatic)
			shadowMap->draw(std::forward<ShadowArgs>(args)...);
	}

	template<typename ...ShadowArgs>
	inline void Light::renderShadowmapForced(ShadowArgs&& ...args) {

		if (shadowMap != nullptr)
			shadowMap->draw(std::forward<ShadowArgs>(args)...);
	}

}

#endif
