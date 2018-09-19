#ifndef LIGHT_H
#define LIGHT_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <functional>
#include <list>
#include <string>
#include "utility/listener.h"
#include "sceneobject.h"
#include "lightmaps.h"

using glm::vec3;

namespace geeL {

	class Camera;
	class ITexture;
	class Shadowmap;
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
	class Light : public SceneObject, public ChangeListener<Transform> {

	public:
		Light(Transform& transform, vec3 diffuse, const std::string& name = "Light");
		virtual ~Light();

		virtual void bind(const Shader& shader, 
			const std::string& name, ShaderTransformSpace space, const Camera* const camera = nullptr) const;

		virtual void bind(const SceneShader& shader,
			const std::string& name, const Camera* const camera = nullptr) const;

		const Shadowmap* const getShadowMap() const;
		Shadowmap* getShadowMap();

		//Set map indicies. Their are needed for the shader to know
		//which light maps to use. Index 0 is equivalent to no index, index n is 
		//equivalent to array position (n - 1)
		virtual void setMapIndex(unsigned int index, LightMapType type) = 0;
		virtual LightMapContainer getMaps() const = 0;
		virtual const ITexture* const getMap(LightMapType type) = 0;

		void attachShadowmap(std::unique_ptr<Shadowmap> map);
		void detatchShadowmap();


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
		
		//States if any light map is currently active.
		//Note: No map is active if no map is 
		//attached or light is currently inactive
		bool hasActiveMaps() const;

		virtual std::string getShadowmapContainerName() const = 0;
		virtual LightType getLightType() const = 0;
		virtual void setActive(bool active);

		//Add change callback to this light and specify whether function should
		//be invoked once immediately or not
		void addChangeListener(std::function<void(Light&)> function, bool invoke = false);

		//Add callback that listens to changes of shadowmap. You can state if callback
		//should be immediately invoked if a shadow map is currently active
		void addShadowmapChangeListener(std::function<void(Light&)> function, bool invoke = true);

	protected:
		unsigned int shadowmapIndex;
		vec3 diffuse;
		Shadowmap* shadowMap;

		virtual void onChange(const Transform& t);
		void onMapChange();

		float getLightRadius(float cutoff) const;

	private:
		float intensity;
		vec3 color;
		std::list<std::function<void(Light&)>> changeListeners;
		std::list<std::function<void(Light&)>> changeMapListeners;

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
