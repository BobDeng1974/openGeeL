#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <functional>
#include <list>
#include <set>
#include <string>
#include <vec3.hpp>
#include "../shadowmapping/shadowmapconf.h"

#define MAX_POINTLIGHTS 5
#define MAX_DIRECTIONALLIGHTS 5
#define MAX_SPOTLIGHTS 5

namespace geeL {

	class Camera;
	class DirectionalLight;
	class DynamicCubeMap;
	class DynamicIBLMap;
	class IBLMap;
	class Light;
	class SceneCamera;
	class PointLight;
	class SpotLight;
	class Shader;
	class ShadowMap;
	class SceneShader;
	class RenderScene;
	class Transform;

	class LightBinding;
	class DLightBinding;
	class PLightBinding;
	class SLightBinding;

	struct ScreenInfo;
	enum class ShaderTransformSpace;


	//Managing class for all light sources in a scene
	class LightManager {

	public:
		std::string plName = "pointLights";
		std::string dlName = "directionalLights";
		std::string slName = "spotLights";
		std::string plCountName = "plCount";
		std::string dlCountName = "dlCount";
		std::string slCountName = "slCount";
		std::string rpCountName = "rpCount";

		LightManager();
		~LightManager();

		//Add and create directional light
		DirectionalLight& addDirectionalLight(const SceneCamera& camera, 
			Transform& transform, glm::vec3 diffuse, ShadowMapConfiguration config);
	
		//Add and create point light
		PointLight& addPointLight(Transform& transform, glm::vec3 diffuse, ShadowMapConfiguration config);
	
		//Add and create spotlight
		SpotLight& addSpotlight(Transform& transform, glm::vec3 diffuse,
			float angle, float outerAngle, ShadowMapConfiguration config);

		void removeLight(DirectionalLight& light);
		void removeLight(PointLight& light);
		void removeLight(SpotLight& light);

		DynamicIBLMap& addReflectionProbe(const DynamicIBLMap& probe);
		IBLMap& addReflectionProbe(const IBLMap& probe);

		void removeReflectionProbe(DynamicCubeMap& probe);
		void bindReflectionProbes(const Camera& camera, const Shader& shader, ShaderTransformSpace space) const;
		void drawReflectionProbes() const;

		void bind(const Camera& camera, const Shader& shader, ShaderTransformSpace space) const;
		void bind(const Camera& camera, const SceneShader& shader) const;

		void bindReflectionProbes(Shader& shader) const;
		void bindShadowMaps(Shader& shader) const;

		void bindShadowmap(Shader& shader, DirectionalLight& light) const;
		void bindShadowmap(Shader& shader, PointLight& light) const;
		void bindShadowmap(Shader& shader, SpotLight& light) const;

		//Draw shadow maps no matter the lights properties. E.g.if they are static or not
		void drawShadowmapsForced(const RenderScene& scene, const SceneCamera* const camera) const;
		void drawShadowmaps(const RenderScene& scene, const SceneCamera* const camera) const;

		void iterDirectionalLights(std::function<void(DirectionalLight&)> function);
		void iterPointLights(std::function<void(PointLight&)> function);
		void iterSpotLights(std::function<void(SpotLight&)> function);

		void addLightAddListener(std::function<void(Light const *, ShadowMap const *)> listener);
		void addLightRemoveListener(std::function<void(Light const *, ShadowMap const *)> listener);

		//Add shader that shall be updated when lights are added/removed
		void addShaderListener(Shader& shader);

		const glm::vec3& getAmbientColor() const;
		void setAmbientColor(const glm::vec3& color);

	private:
		glm::vec3 ambient;
		Shader* dlShader; //Shader for spot and directional light shadow map
		Shader* plShader; //Shader for point light shadow maps

		std::list<DLightBinding> dirLights;
		std::list<SLightBinding> spotLights;
		std::list<PLightBinding> pointLights;
		std::list<DynamicCubeMap*> reflectionProbes;

		std::list<std::function<void(Light const *, ShadowMap const *)>> addListener;
		std::list<std::function<void(Light const *, ShadowMap const *)>> removeListener;
		std::set<Shader*> shaderListener;

		void onRemove(Light* light, LightBinding& binding);
		void onAdd(Light* light, LightBinding& binding);

		template<class B, class L, class A>
		const B* getBinding(const L& light, const A& list) const;

		template<class B, class L, class A>
		B* getBinding(const L& light, A& list);

		template<class A>
		void reindexLights(A& list) const;
	};
}

#endif
