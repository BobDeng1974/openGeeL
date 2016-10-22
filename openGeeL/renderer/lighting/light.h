#ifndef LIGHT_H
#define LIGHT_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <string>
#include "../sceneobject.h"


using glm::vec3;

namespace geeL {

class Transform;
class RenderScene;
class Shader;

class Light : public SceneObject {

public:
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;
	float intensity;
	

	Light(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient, float intensity) 
		: SceneObject(transform), diffuse(diffuse), specular(specular), ambient(ambient), intensity(intensity) {}

	virtual void bind(const Shader& shader, int index, std::string name) const;

	virtual void initShadowmap();
	virtual void addShadowmap(Shader& shader, std::string name = "shadowMap");
	virtual void renderShadowmap(const RenderScene& scene, const Shader& shader);
	virtual void computeLightTransform() = 0;

	const int getShadowMapID() const;
	const int getShadowMapFBO() const;

protected:
	unsigned int shadowmapFBO;
	unsigned int shadowmapID;
	int shadowmapHeight;
	int shadowmapWidth;
	glm::mat4 lightTransform;

};

}

#endif
