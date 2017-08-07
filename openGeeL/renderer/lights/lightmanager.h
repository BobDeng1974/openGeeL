#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <functional>
#include <list>
#include <set>
#include <string>
#include <vec3.hpp>
#include "shadowmapping/shadowmapconfig.h"

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
	class RenderShader;
	class ShadowMap;
	class SceneShader;
	class RenderScene;
	class Transform;
	class VoxelStructure;

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
			Transform& transform, glm::vec3 diffuse, const ShadowMapConfiguration& config);
	
		//Add and create point light
		PointLight& addPointLight(Transform& transform, glm::vec3 diffuse, const ShadowMapConfiguration& config);
	
		//Add and create spotlight
		SpotLight& addSpotlight(Transform& transform, glm::vec3 diffuse,
			float angle, float outerAngle, const ShadowMapConfiguration& config);

		void removeLight(DirectionalLight& light);
		void removeLight(PointLight& light);
		void removeLight(SpotLight& light);

		DynamicIBLMap& addReflectionProbe(const DynamicIBLMap& probe);
		IBLMap& addReflectionProbe(const IBLMap& probe);

		void removeReflectionProbe(DynamicCubeMap& probe);

		//Add all reflection probes to given shader
		void addReflectionProbes(RenderShader& shader) const;
		void bindReflectionProbes(const Camera& camera, const RenderShader& shader, ShaderTransformSpace space) const;
		void drawReflectionProbes() const;

		void bind(const RenderShader& shader, ShaderTransformSpace space, const Camera* const camera = nullptr) const;
		void bind(const SceneShader& shader, const Camera* const camera = nullptr) const;

		void bindShadowMaps(RenderShader& shader) const;

		void bindShadowmap(RenderShader& shader, DirectionalLight& light) const;
		void bindShadowmap(RenderShader& shader, PointLight& light) const;
		void bindShadowmap(RenderShader& shader, SpotLight& light) const;

		//Update all internal structures depending on their state
		void draw(const RenderScene& scene, const SceneCamera* const camera);

		//Draw shadow maps no matter the lights properties. E.g.if they are static or not
		void drawShadowmapsForced(const RenderScene& scene, const SceneCamera* const camera) const;
		void drawShadowmaps(const RenderScene& scene, const SceneCamera* const camera) const;

		void addVoxelStructure(VoxelStructure& structure);
		void drawVoxelStructure();

		void iterLights(std::function<void(Light&)> function);
		void iterDirectionalLights(std::function<void(DirectionalLight&)> function);
		void iterPointLights(std::function<void(PointLight&)> function);
		void iterSpotLights(std::function<void(SpotLight&)> function);

		void addLightAddListener(std::function<void(Light const *, ShadowMap const *)> listener);
		void addLightRemoveListener(std::function<void(Light const *, ShadowMap const *)> listener);

		//Add shader that shall be updated when lights are added/removed
		void addShaderListener(RenderShader& shader);

		const glm::vec3& getAmbientColor() const;
		void setAmbientColor(const glm::vec3& color);

	private:
		glm::vec3 ambient;
		VoxelStructure* voxelStructure;
		RenderShader* dlShader; //RenderShader for spot and directional light shadow map
		RenderShader* plShader; //RenderShader for point light shadow maps

		std::list<DLightBinding> dirLights;
		std::list<SLightBinding> spotLights;
		std::list<PLightBinding> pointLights;
		std::list<DynamicCubeMap*> reflectionProbes;

		std::list<std::function<void(Light const *, ShadowMap const *)>> addListener;
		std::list<std::function<void(Light const *, ShadowMap const *)>> removeListener;
		std::set<RenderShader*> shaderListener;

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
