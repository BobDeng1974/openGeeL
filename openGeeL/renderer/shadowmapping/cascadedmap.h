#ifndef CASCADEDSHADOWMAP_H
#define CASCADEDSHADOWMAP_H

#include <vec3.hpp>
#include <mat4x4.hpp>
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
		CascadedShadowMap(const Light& light, float shadowBias, unsigned int resolution);

		virtual Resolution getScreenResolution() const;

	protected:
		unsigned int resolution;
		float shadowBias;

		CascadedShadowMap(const CascadedShadowMap& other) = delete;
		CascadedShadowMap& operator= (const CascadedShadowMap& other) = delete;

	};


	class CascadedDirectionalShadowMap : public CascadedShadowMap {

	public:
		CascadedDirectionalShadowMap(const Light& light, const SceneCamera& camera, 
			float shadowBias, unsigned int resolution);

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



	inline CascadedShadowMap::CascadedShadowMap(const Light& light, 
		float shadowBias, 
		unsigned int resolution)
			: ShadowMap(light)
			, shadowBias(shadowBias)
			, resolution(resolution) {}


}

#endif
