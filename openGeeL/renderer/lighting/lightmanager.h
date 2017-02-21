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
	class RenderScene;
	class Transform;
	struct ScreenInfo;

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
		DirectionalLight& addDirectionalLight(Transform& transform, glm::vec3 diffuse, 
			float shadowBias = 0.00002f);
	
		//Add and create point light
		PointLight& addPointLight(Transform& transform, glm::vec3 diffuse,
			float shadowBias = 0.002f);
	
		//Add and create spotlight
		SpotLight& addSpotlight(Transform& transform, glm::vec3 diffuse,
			float angle, float outerAngle, float shadowBias = 0.0017f);


		void deferredBind(const RenderScene& scene, const Shader& shader) const;
		void forwardBind(const Shader& shader) const;

		void bindShadowmaps(Shader& shader) const;
		void drawShadowmaps(const RenderScene& scene) const;

		void forwardScreenInfo(const ScreenInfo& info, const Camera& camera);
	

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
