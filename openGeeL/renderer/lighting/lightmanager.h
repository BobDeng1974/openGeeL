#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <vector>
#include <vec3.hpp>

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


		void bind(const RenderScene& scene, const Shader& shader, ShaderTransformSpace space) const;
		void bind(const RenderScene& scene, const SceneShader& shader) const;

		void bindShadowmaps(Shader& shader) const;
		void drawShadowmaps(const RenderScene& scene) const;

		std::vector<DirectionalLight*>::iterator directionalLightsBegin();
		std::vector<DirectionalLight*>::iterator directionalLightsEnd();

		std::vector<SpotLight*>::iterator spotLightsBegin();
		std::vector<SpotLight*>::iterator spotLightsEnd();

		std::vector<PointLight*>::iterator pointLightsBegin();
		std::vector<PointLight*>::iterator pointLightsEnd();
	

	private:
		//Shader for spotlights and directional lights
		Shader* dlShader;
		Shader* plShader;

		std::vector<DirectionalLight*> staticDLs;
		std::vector<SpotLight*> staticSLs;
		std::vector<PointLight*> staticPLs;
	};
}

#endif
