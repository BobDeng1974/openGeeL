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
	const RenderScene;

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

		void renderShadowmap(const SceneCamera* const camera, 
			std::function<void(const RenderShader&)> renderCall, const ShadowmapRepository& repository);

		//Draw shadow map no matter the lights properties. E.g. if it is static or not
		void renderShadowmapForced(const SceneCamera* const camera,
			std::function<void(const RenderShader&)> renderCall, const ShadowmapRepository& repository);

		//Computes experienced intensity at given point. Ranges between 0 and 1
		virtual float getIntensity(glm::vec3 point) const;

		//Returns diffuse color of this light
		vec3 getColor() const;
		void setColor(vec3 color);

		//Add change callback to this light and specify whether function should
		//be invoked once immediately or not
		void addChangeListener(std::function<void(Light&)> function, bool invoke = false);

		virtual LightType getLightType() const = 0;

	protected:
		vec3 diffuse;
		unsigned int shadowmapFBO;
		unsigned int shadowmapID;
		ShadowMap* shadowMap;

		void onChange();

	private:
		std::list<std::function<void(Light&)>> changeListeners;

	};
}

#endif
