#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <vector>
#include <vec3.hpp>

#define MAX_POINTLIGHTS 5
#define MAX_DIRECTIONALLIGHTS 5
#define MAX_SPOTLIGHTS 5

namespace geeL {

class Light;
class PointLight;
class DirectionalLight;
class SpotLight;
class Shader;
class RenderScene;
class Transform;

class LightManager {

public:
	std::string plName = "pointLights";
	std::string dlName = "directionalLights";
	std::string slName = "spotLights";
	std::string plCountName = "plCount";
	std::string dlCountName = "dlCount";
	std::string slCountName = "slCount";

	LightManager();

	//Manager is responsible for removing the lights
	~LightManager();

	//Add and create directional light
	DirectionalLight& addLight(Transform& transform, glm::vec3 diffuse, glm::vec3 specular, 
		glm::vec3 ambient, float intensity);
	
	//Add and create point light
	PointLight& addLight(Transform& transform, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 ambient,
		float intensity, float attenuation);
	
	//Add and create spotlight
	SpotLight& addLight(Transform& transform, glm::vec3 diffuse, glm::vec3 specular,
		glm::vec3 ambient, float angle, float outerAngle, float intensity);


	void bind(const Shader& shader) const;
	void bindShadowmaps(Shader& shader) const;
	void drawShadowmaps(const RenderScene& scene) const;
	

private:
	Shader* dlShader;

	std::vector<Light*> staticPLs, staticDLs, staticSLs;
	std::vector<Light*> dynamicPLs, dynamicDLs, dynamicSLs;
};

}

#endif
