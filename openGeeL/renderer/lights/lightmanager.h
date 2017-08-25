#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <functional>
#include <list>
#include <set>
#include <map>
#include <string>
#include <vec3.hpp>
#include "shadowmapping/shadowmapconfig.h"
#include "utility/worldinformation.h"

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
		DirectionalLight& addDirectionalLight(const SceneCamera& camera, 
			Transform& transform, glm::vec3 diffuse, const ShadowMapConfiguration& config);
	
		//Add and create point light
		PointLight& addPointLight(Transform& transform, glm::vec3 diffuse, const ShadowMapConfiguration& config);
	
		//Add and create spotlight
		SpotLight& addSpotlight(Transform& transform, glm::vec3 diffuse,
			float angle, float outerAngle, const ShadowMapConfiguration& config);

		void removeLight(Light& light);


		void bind(const RenderShader& shader, ShaderTransformSpace space, const Camera* const camera = nullptr) const;
		void bind(const SceneShader& shader, const Camera* const camera = nullptr) const;
		void bindShadowmaps(RenderShader& shader) const;

		//Update all internal structures depending on their state
		void update(const RenderScene& scene, const SceneCamera* const camera);

		//Draw shadow maps no matter the lights properties. E.g.if they are static or not
		void drawShadowmapsForced(const RenderScene& scene, const SceneCamera* const camera) const;
		

		void iterLights(std::function<void(Light&)> function);
		void iterLights(std::function<void(LightBinding&)> function);
		void iterLights(std::function<void(const LightBinding&)> function) const;


		DynamicIBLMap& addReflectionProbe(const DynamicIBLMap& probe);
		IBLMap& addReflectionProbe(const IBLMap& probe);
		void removeReflectionProbe(DynamicCubeMap& probe);

		//Add all reflection probes to given shader
		void addReflectionProbes(RenderShader& shader) const;
		void bindReflectionProbes(const Camera& camera, const RenderShader& shader, ShaderTransformSpace space) const;
		void drawReflectionProbes() const;

		void addVoxelStructure(VoxelStructure& structure);
		void drawVoxelStructure();

		
		//Add shader that shall be updated when lights are added/removed
		void addShaderListener(RenderShader& shader);
		void addShaderListener(SceneShader& shader);

		const glm::vec3& getAmbientColor() const;
		void setAmbientColor(const glm::vec3& color);

	private:
		glm::vec3 ambient;
		VoxelStructure* voxelStructure;
		ShadowmapRepository shaderRepository;
		size_t plCount, dlCount, slCount;

		std::map<Light*, LightBinding> lights;
		std::list<DynamicCubeMap*> reflectionProbes;
		std::set<RenderShader*> shaderListener;

		void addLight(Light* light, LightBinding& binding);

		void onRemove(Light* light, LightBinding& binding);
		void onAdd(Light* light, LightBinding& binding);
		void onChange(Light& light);

		void drawShadowmaps(const RenderScene& scene, const SceneCamera* const camera) const;
		void reindexLights();

	};
}

#endif
