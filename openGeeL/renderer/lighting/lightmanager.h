#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <functional>
#include <list>
#include <string>
#include <vector>
#include <vec3.hpp>
#include "lightbinding.h"

#define MAX_POINTLIGHTS 5
#define MAX_DIRECTIONALLIGHTS 5
#define MAX_SPOTLIGHTS 5

namespace geeL {

	class Light;
	class Camera;
	class PointLight;
	class DirectionalLight;
	class SpotLight;
	class Shader;
	class ShadowMap;
	class SceneShader;
	class RenderScene;
	class Transform;

	struct ScreenInfo;
	enum class ShaderTransformSpace;


	class LightManager {

	public:
		glm::vec3 ambient;
		std::string plName = "pointLights";
		std::string dlName = "directionalLights";
		std::string slName = "spotLights";
		std::string plCountName = "plCount";
		std::string dlCountName = "dlCount";
		std::string slCountName = "slCount";

		LightManager(glm::vec3 ambient = glm::vec3(0.25f));

		//Manager is responsible for removing the lights
		~LightManager();

		//Add and create directional light
		DirectionalLight& addDirectionalLight(const Camera& camera, Transform& transform, glm::vec3 diffuse,
			float shadowBias = 0.00002f);
	
		//Add and create point light
		PointLight& addPointLight(Transform& transform, glm::vec3 diffuse,
			float shadowBias = 0.001f);
	
		//Add and create spotlight
		SpotLight& addSpotlight(Transform& transform, glm::vec3 diffuse,
			float angle, float outerAngle, float shadowBias = 0.001f);

		void removeLight(DirectionalLight& light);
		void removeLight(PointLight& light);
		void removeLight(SpotLight& light);

		void bind(const RenderScene& scene, const Shader& shader, ShaderTransformSpace space) const;
		void bind(const RenderScene& scene, const SceneShader& shader) const;

		void bindShadowmap(Shader& shader, DirectionalLight& light);
		void bindShadowmap(Shader& shader, PointLight& light);
		void bindShadowmap(Shader& shader, SpotLight& light);

		void bindShadowmaps(Shader& shader) const;
		void drawShadowmaps(const RenderScene& scene) const;

		void iterDirectionalLights(std::function<void(DirectionalLight&)> function);
		void iterPointLights(std::function<void(PointLight&)> function);
		void iterSpotLights(std::function<void(SpotLight&)> function);

		void addLightAddListener(std::function<void(Light const *, ShadowMap const *)> listener);
		void addLightRemoveListener(std::function<void(Light const *, ShadowMap const *)> listener);

	private:
		//Shader for spotlights and directional lights
		Shader* dlShader;
		Shader* plShader;

		std::list<DLightBinding> dirLights;
		std::list<SLightBinding> spotLights;
		std::list<PLightBinding> pointLights;

		std::list<std::function<void(Light const *, ShadowMap const *)>> addListener;
		std::list<std::function<void(Light const *, ShadowMap const *)>> removeListener;

		void onRemove(Light* light);
		void onAdd(Light* light);

		template<class B, class L, class A>
		B* getBinding(L& light, A& list);

		template<class A>
		void reindexLights(A& list);
	};
}

#endif
