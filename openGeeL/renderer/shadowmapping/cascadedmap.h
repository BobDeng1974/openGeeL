#ifndef CASCADEDSHADOWMAP_H
#define CASCADEDSHADOWMAP_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include "shadowmapconfig.h"
#include "shadowmap.h"

#define MAPCOUNT 4

namespace geeL {

	struct CascadedMap {
		float cascadeEnd;
		float cascadeEndClip;

		glm::mat4 lightTransform;
	};


	class CascadedShadowMap : public ShadowMap {

	public:
		CascadedShadowMap(const Light& light, std::unique_ptr<Texture> innerTexture, 
			float shadowBias, ShadowmapResolution resolution);

		virtual Resolution getScreenResolution() const;

	protected:
		float shadowBias;

		CascadedShadowMap(const CascadedShadowMap& other) = delete;
		CascadedShadowMap& operator= (const CascadedShadowMap& other) = delete;

	};


	class CascadedDirectionalShadowMap : public CascadedShadowMap {

	public:
		CascadedDirectionalShadowMap(const Light& light, const SceneCamera& camera, 
			float shadowBias, ShadowmapResolution resolution);

		virtual void bindData(const Shader& shader, const std::string& name);
		virtual void removeMap(Shader& shader);

		virtual void draw(const SceneCamera* const camera, const RenderScene& scene,
			ShadowmapRepository& repository);

		//Set split planes (between cameras near and far clip plane)
		void setCascades(const SceneCamera& camera);

		virtual TextureType getTextureType() const;

	private:
		CascadedMap shadowMaps[MAPCOUNT];

		void computeLightTransforms(const SceneCamera& camera);
	};

}

#endif
