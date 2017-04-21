#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <functional>
#include <list>
#include <set>
#include <string>
#include <vec3.hpp>

#define MAX_POINTLIGHTS 5
#define MAX_DIRECTIONALLIGHTS 5
#define MAX_SPOTLIGHTS 5



namespace geeL {

	class Camera;
	class Light;
	class SceneCamera;
	class PointLight;
	class DirectionalLight;
	class SpotLight;
	class Shader;
	class ShadowMap;
	class SceneShader;
	class ReflectionProbe;
	class RenderScene;
	class Transform;

	class LightBinding;
	class DLightBinding;
	class PLightBinding;
	class SLightBinding;

	struct ScreenInfo;
	struct FrameBufferInformation;
	enum class ShaderTransformSpace;

	typedef std::function<void(const Camera&, FrameBufferInformation)> ReflectionProbeRender;


	//Managing class for all light sources in a scene
	class LightManager {

	public:
		glm::vec3 ambient;
		std::string plName = "pointLights";
		std::string dlName = "directionalLights";
		std::string slName = "spotLights";
		std::string plCountName = "plCount";
		std::string dlCountName = "dlCount";
		std::string slCountName = "slCount";

		LightManager(ReflectionProbeRender reflectionProbeRenderCall,
			glm::vec3 ambient = glm::vec3(0.25f));

		//Manager is responsible for removing the lights
		~LightManager();

		//Add and create directional light
		DirectionalLight& addDirectionalLight(const SceneCamera& camera, Transform& transform, glm::vec3 diffuse,
			float shadowBias = 0.00002f);
	
		//Add and create point light
		PointLight& addPointLight(Transform& transform, glm::vec3 diffuse,
			float shadowBias = 0.0001f);
	
		//Add and create spotlight
		SpotLight& addSpotlight(Transform& transform, glm::vec3 diffuse,
			float angle, float outerAngle, float shadowBias = 0.0001f);

		void removeLight(DirectionalLight& light);
		void removeLight(PointLight& light);
		void removeLight(SpotLight& light);

		ReflectionProbe& addReflectionProbe(Transform& transform, float depth, unsigned int resolution);
		void removeReflectionProbe(ReflectionProbe& probe);
		void drawReflectionProbes() const;

		void bind(const Camera& camera, const Shader& shader, ShaderTransformSpace space) const;
		void bind(const Camera& camera, const SceneShader& shader) const;

		void bindShadowmap(Shader& shader, DirectionalLight& light) const;
		void bindShadowmap(Shader& shader, PointLight& light) const;
		void bindShadowmap(Shader& shader, SpotLight& light) const;

		void bindShadowmaps(Shader& shader) const;
		void drawShadowmaps(const RenderScene& scene, const SceneCamera* const camera) const;

		void iterDirectionalLights(std::function<void(DirectionalLight&)> function);
		void iterPointLights(std::function<void(PointLight&)> function);
		void iterSpotLights(std::function<void(SpotLight&)> function);

		void addLightAddListener(std::function<void(Light const *, ShadowMap const *)> listener);
		void addLightRemoveListener(std::function<void(Light const *, ShadowMap const *)> listener);

		//Add shader that shall be updated when lights are added/removed
		void addShaderListener(Shader& shader);

	private:
		//Shader for spotlights and directional lights
		Shader* dlShader;
		Shader* plShader;
		ReflectionProbeRender renderCall;

		std::list<DLightBinding> dirLights;
		std::list<SLightBinding> spotLights;
		std::list<PLightBinding> pointLights;
		std::list<ReflectionProbe*> reflectionProbes;

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
