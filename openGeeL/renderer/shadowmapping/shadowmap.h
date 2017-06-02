#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "shadowmapconfig.h"
#include "../lights/light.h"

namespace geeL {

	class ShadowMap {

	public:
		ShadowMap(const Light& light, ShadowMapType type = ShadowMapType::Soft) 
			: light(light), type(type) {}


		virtual void bindData(const RenderShader& shader, const std::string& name) = 0;
		virtual void bindMap(RenderShader& shader, const std::string& name) = 0;
		virtual void removeMap(RenderShader& shader) = 0;

		//Render function for shadow maps. Takes current scene camera, render function
		//that  draws desired objects of scene and the actual shadow map shader.
		//Note: Scene camera can be NULL so a fallback strategy needs to be implemented
		virtual void draw(const SceneCamera* const camera,
			std::function<void(const RenderShader&)> renderCall, const RenderShader& shader) = 0;

		virtual unsigned int getID() const = 0;

		ShadowMapType getType() const {
			return type;
		}

	protected:
		const ShadowMapType type;
		const Light& light;

	};
	
}

#endif