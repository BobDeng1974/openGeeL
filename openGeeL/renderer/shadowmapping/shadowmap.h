#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include <memory>
#include "framebuffer/fbotoken.h"
#include "texturing/texture.h"
#include "shadowmapconfig.h"
#include "lights/light.h"
#include "shadowbuffer.h"

namespace geeL {

	class RenderScene;


	class ShadowMap : public Texture {

	public:
		ShadowMap(const Light& light, std::unique_ptr<Texture> innerTexture);
		virtual ~ShadowMap();


		virtual void bindData(const Shader& shader, const std::string& name) = 0;

		//Render function for shadow maps. Takes current scene, a camera and the actual shadow map shader.
		//Note: Scene camera can be NULL so a fallback strategy needs to be implemented
		virtual void draw(const SceneCamera* const camera, const RenderScene& scene, 
			ShadowmapRepository& repository) = 0;

		float getIntensity() const;
		void setIntensity(float value);

		virtual ShadowMapType getType() const;
		virtual TextureType getTextureType() const;
		virtual unsigned int getID() const;

	protected:
		const Light& light;
		DepthFrameBuffer buffer;
		float intensity;

		Texture& getInnerTexture();

	private:
		Texture* texture;

	};


	inline ShadowMap::ShadowMap(const Light& light, std::unique_ptr<Texture> innerTexture)
		: Texture(ColorType::Depth)
		, buffer(Resolution(500))
		, light(light)
		, intensity(1.f) 
		, texture(innerTexture.release()) {
	
		texture->setBorderColors(1.f, 1.f, 1.f, 1.f);
	}


	inline ShadowMap::~ShadowMap() {
		delete texture;
	}


	inline float ShadowMap::getIntensity() const {
		return intensity;
	}

	inline void ShadowMap::setIntensity(float value) {
		if (intensity != value && value > 0.f && value < 1.f)
			intensity = value;
	}

	inline ShadowMapType ShadowMap::getType() const {
		return ShadowMapType::None;
	}

	inline TextureType ShadowMap::getTextureType() const {
		return texture->getTextureType();
	}

	inline unsigned int ShadowMap::getID() const {
		return texture->getID();
	}

	inline Texture& ShadowMap::getInnerTexture() {
		return *texture;
	}
	
}

#endif