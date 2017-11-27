#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "framebuffer/fbotoken.h"
#include "texturing/texture.h"
#include "shadowmapconfig.h"
#include "lights/light.h"

namespace geeL {

	class ShadowMap : public Texture {

	public:
		ShadowMap(const Light& light, ShadowMapType type = ShadowMapType::Soft) 
			: Texture(ColorType::None), light(light), type(type), intensity(1.f) {}

		virtual void bindData(const Shader& shader, const std::string& name) = 0;
		virtual void removeMap(Shader& shader) = 0;

		//Render function for shadow maps. Takes current scene camera, render function
		//that  draws desired objects of scene and the actual shadow map shader.
		//Note: Scene camera can be NULL so a fallback strategy needs to be implemented
		virtual void draw(const SceneCamera* const camera,
			std::function<void(const RenderShader&)> renderCall, ShadowmapRepository& repository) = 0;

		virtual TextureType getTextureType() const = 0;

		float getIntensity() const;
		void setIntensity(float value);

		ShadowMapType getType() const;

	protected:
		FrameBufferToken fbo;
		float intensity;
		const ShadowMapType type;
		const Light& light;

	};


	inline float ShadowMap::getIntensity() const {
		return intensity;
	}

	inline void ShadowMap::setIntensity(float value) {
		if (intensity != value && value > 0.f && value < 1.f)
			intensity = value;
	}

	inline ShadowMapType ShadowMap::getType() const {
		return type;
	}
	
}

#endif