#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <functional>
#include <list>
#include <set>
#include <map>
#include <memory>
#include <string>
#include <vec3.hpp>
#include "shadowmapping/shadowmapconfig.h"
#include "utility/worldinformation.h"
#include "pointlight.h"
#include "directionallight.h"
#include "spotlight.h"

#define MAX_POINTLIGHTS 5
#define MAX_DIRECTIONALLIGHTS 5
#define MAX_SPOTLIGHTS 5


namespace geeL {

	class Camera;
	class DynamicCubeMap;
	class DynamicIBLMap;
	class IBLMap;
	class Light;
	class SceneCamera;
	class RenderShader;
	class ShadowMap;
	class SceneShader;
	class RenderScene;
	class Shader;
	class ShadowMap;
	class ShadowmapAdapter;
	class Transform;
	class VoxelStructure;
	class LightBinding;

	struct ScreenInfo;
	enum class ShaderTransformSpace;


	//Managing class for all light sources in a scene
	class LightManager : public CameraRequester {

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
		template<typename ...DirectonalLightArgs>
		DirectionalLight& addDirectionalLight(const ShadowMapConfiguration& config, DirectonalLightArgs&& ...args);
	
		//Add and create point light
		template<typename ...PointLightArgs>
		PointLight& addPointLight(const ShadowMapConfiguration& config, PointLightArgs&& ...args);
	
		//Add and create spotlight
		template<typename ...SpotLightArgs>
		SpotLight& addSpotlight(const ShadowMapConfiguration& config, SpotLightArgs&& ...args);

		void removeLight(Light& light);


		void bind(const Shader& shader, ShaderTransformSpace space, const Camera* const camera = nullptr) const;
		void bind(const SceneShader& shader, const Camera* const camera = nullptr) const;
		void bindShadowmaps(Shader& shader) const;

		//Update all internal structures depending on their state
		void update(const RenderScene& scene, const SceneCamera* const camera);

		//Draw shadow maps no matter the lights properties. E.g.if they are static or not
		void drawShadowmapsForced(const RenderScene& scene, const SceneCamera* const camera);
		

		void iterLights(std::function<void(Light&)> function);
		void iterLights(std::function<void(LightBinding&)> function);
		void iterLights(std::function<void(const LightBinding&)> function) const;
		void iterShadowmaps(std::function<void(ShadowMap&)> function) const;


		DynamicIBLMap& addReflectionProbe(DynamicIBLMap& probe);
		IBLMap& addReflectionProbe(IBLMap& probe);
		void removeReflectionProbe(DynamicCubeMap& probe);

		//Add all reflection probes to given shader
		void addReflectionProbes(RenderShader& shader) const;
		void removeReflectionProbes(RenderShader& shader) const;

		void bindReflectionProbes(const Camera& camera, const RenderShader& shader, ShaderTransformSpace space) const;
		void drawReflectionProbes() const;

		void addVoxelStructure(VoxelStructure& structure);
		void drawVoxelStructure();

		//Returns shadowmap adapter if attached, null otherwise
		ShadowmapAdapter* const getShadowmapAdapter();
		void addShadowmapAdapter(ShadowmapAdapter& manager);

		//Add shader that shall be updated when lights are added/removed
		void addShaderListener(Shader& shader);
		void addShaderListener(SceneShader& shader);

		const glm::vec3& getAmbientColor() const;
		void setAmbientColor(const glm::vec3& color);

	private:
		glm::vec3 ambient;
		VoxelStructure* voxelStructure;
		ShadowmapAdapter* mapAdapter;

		ShadowmapRepository shaderRepository;
		size_t plCount, dlCount, slCount;

		std::map<Light*, LightBinding> lights;
		std::list<DynamicCubeMap*> reflectionProbes;
		std::set<Shader*> shaderListener;

		void addLight(Light* light, LightBinding& binding);
		void addDirectionalLightInternal(DirectionalLight* light, const ShadowMapConfiguration& config);
		void addPointLightInternal(PointLight* light, const ShadowMapConfiguration& config);
		void addSpotlightInternal(SpotLight* light, const ShadowMapConfiguration& config);


		void onRemove(Light* light, LightBinding& binding);
		void onAdd(Light* light, LightBinding& binding);
		void onChange(Light& light);

		void drawShadowmaps(const RenderScene& scene, const SceneCamera* const camera);
		void reindexLights();

	};



	template<typename ...DirectonalLightArgs>
	inline DirectionalLight& LightManager::addDirectionalLight(const ShadowMapConfiguration& config, DirectonalLightArgs&& ...args) {
		DirectionalLight* light = new DirectionalLight(std::forward<DirectonalLightArgs>(args)...);
		addDirectionalLightInternal(light, config);

		return *light;
	}

	template<typename ...PointLightArgs>
	inline PointLight& LightManager::addPointLight(const ShadowMapConfiguration& config, PointLightArgs&& ...args) {
		PointLight* light = new PointLight(std::forward<PointLightArgs>(args)...);
		addPointLightInternal(light, config);

		return *light;
	}

	template<typename ...SpotLightArgs>
	inline SpotLight& LightManager::addSpotlight(const ShadowMapConfiguration& config, SpotLightArgs&& ...args) {
		SpotLight* light = new SpotLight(std::forward<SpotLightArgs>(args)...);
		addSpotlightInternal(light, config);

		return *light;
	}

}

#endif
