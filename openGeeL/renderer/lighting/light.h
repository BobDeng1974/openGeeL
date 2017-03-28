#ifndef LIGHT_H
#define LIGHT_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <functional>
#include <string>
#include "../sceneobject.h"

using glm::vec3;

namespace geeL {

	class ShadowMap;
	class Camera;
	class Transform;
	class RenderScene;
	class Shader;
	class SceneShader;
	const RenderScene;

	enum class ShaderTransformSpace;
	


	class Light : public SceneObject {

	public:
		vec3 diffuse;

		Light(Transform& transform, vec3 diffuse, const std::string& name = "Light");
		~Light();

		virtual void bind(const Camera& camera, const Shader& shader, 
			const std::string& name, ShaderTransformSpace space) const;

		virtual void bind(const Camera& camera, const SceneShader& shader,
			const std::string& name) const;

		const ShadowMap* const getShadowMap() const;
		ShadowMap* const getShadowMap();
		void setShadowMap(ShadowMap& map);

		//Add shadow map to given shader
		virtual void addShadowmap(Shader& shader, const std::string& name);

		//Remove shadow map from given shader
		virtual void removeShadowmap(Shader& shader);

		virtual void renderShadowmap(const Camera& camera, 
			std::function<void(const Shader&)> renderCall, const Shader& shader);

		//Computes experienced intensity at given point. Ranges between 0 and 1
		virtual float getIntensity(glm::vec3 point) const;


	protected:
		unsigned int shadowmapFBO;
		unsigned int shadowmapID;

		ShadowMap* shadowMap;

	};
}

#endif
