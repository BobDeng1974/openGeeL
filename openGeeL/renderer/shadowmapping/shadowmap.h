#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include <memory>
#include "framebuffer/fbotoken.h"
#include "texturing/functionaltexture.h"
#include "shadowmapconfig.h"
#include "lights/light.h"
#include "shadowbuffer.h"

namespace geeL {

	class RenderScene;


	class Shadowmap : public FunctionalTexture {

	public:
		//ShadowMap(const Light& light, std::unique_ptr<Texture> innerTexture);
		Shadowmap(const Light& light, std::unique_ptr<Texture> innerTexture,
			ShadowmapResolution targetResolution);

		virtual void bindData(const Shader& shader, const std::string& name) = 0;

		//Resize shadow map with given resolution.
		//Note: This will only change the resolution of the underlying texture but not the 'targeted'
		//shadow map resolution. For that, 'setShadowResolution' has to be called
		virtual void resize(ShadowmapResolution resolution);

		//Render function for shadow maps. Takes current scene, a camera and the actual shadow map shader.
		//Note: Scene camera can be NULL so a fallback strategy needs to be implemented
		virtual void draw(const SceneCamera* const camera, const RenderScene& scene, 
			ShadowmapRepository& repository) = 0;

		//Sets 'targeted' shadow map resolution and resizes underlying texture
		void setShadowResolution(ShadowmapResolution resolution);
		ShadowmapResolution getShadowResolution() const;

		float getIntensity() const;
		void setIntensity(float value);

		virtual ShadowMapType getType() const;
		const Light& getLight() const;

	protected:
		const Light& light;
		DepthFrameBuffer buffer;
		float intensity;

	private:
		ShadowmapResolution shadowResolution;

	};


	inline Shadowmap::Shadowmap(const Light & light, std::unique_ptr<Texture> innerTexture, ShadowmapResolution targetResolution) 
		: FunctionalTexture(std::move(innerTexture))
		, light(light)
		, intensity(1.f)
		, shadowResolution(targetResolution) {

		Texture& texture = getTexture();
		texture.setBorderColors(1.f, 1.f, 1.f, 1.f);

		Resolution res = texture.getScreenResolution();
		assert(res.getWidth() == res.getHeight());
	}

	inline void Shadowmap::resize(ShadowmapResolution resolution) {
		Texture& texture = getTexture();
		unsigned int res = texture.getScreenResolution().getWidth();
		unsigned int newRes = (unsigned int)resolution;

		if (res != newRes)
			texture.resize(Resolution(newRes));
	}


	inline void Shadowmap::setShadowResolution(ShadowmapResolution resolution) {
		shadowResolution = resolution;
		resize(resolution);
	}

	inline ShadowmapResolution Shadowmap::getShadowResolution() const {
		return shadowResolution;
	}

	inline float Shadowmap::getIntensity() const {
		return intensity;
	}

	inline void Shadowmap::setIntensity(float value) {
		if (intensity != value && value > 0.f && value < 1.f)
			intensity = value;
	}

	inline ShadowMapType Shadowmap::getType() const {
		return ShadowMapType::None;
	}

	inline const Light& Shadowmap::getLight() const {
		return light;
	}

}

#endif