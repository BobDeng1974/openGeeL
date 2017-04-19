#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "../lighting/light.h"

namespace geeL {

	class ShadowMap {

	public:
		ShadowMap(const Light& light) : light(light) {}

		virtual void bindData(const Shader& shader, const std::string& name) = 0;
		virtual void bindMap(Shader& shader, const std::string& name) = 0;
		virtual void removeMap(Shader& shader) = 0;

		//Render function for shadow maps. Takes current scene camera, render function
		//that  draws desired objects of scene and the actual shadow map shader.
		//Note: Scene camera can be NULL so a fallback strategy needs to be implemented
		virtual void draw(const SceneCamera* const camera,
			std::function<void(const Shader&)> renderCall, const Shader& shader) = 0;

		virtual unsigned int getID() const = 0;

	protected:
		const Light& light;

	};
}

#endif