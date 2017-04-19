#ifndef CASCADEDSHADOWMAP_H
#define CASCADEDSHADOWMAP_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include "shadowmap.h"

#define MAPCOUNT 3

namespace geeL {

	struct CascadedMap {
		float cascadeEnd;
		float cascadeEndClip;

		glm::mat4 lightTransform;
	};


	class CascadedShadowMap : public ShadowMap {

	public:
		CascadedShadowMap(const Light& light, float shadowBias, unsigned int width, unsigned int height) 
			: ShadowMap(light), shadowBias(shadowBias), width(width), height(height) {}

		virtual void bindData(const Shader& shader, const std::string& name) = 0;
		virtual void bindMap(Shader& shader, const std::string& name) = 0;
		virtual void removeMap(Shader& shader) = 0;

		virtual void draw(const SceneCamera* const camera,
			std::function<void(const Shader&)> renderCall, const Shader& shader) = 0;

		//Returns ID of first (nearest) shadow map
		virtual unsigned int getID() const = 0;

	protected:
		unsigned int fbo, width, height;
		float shadowBias;

	};


	class CascadedDirectionalShadowMap : public CascadedShadowMap {

	public:
		CascadedDirectionalShadowMap(const Light& light, const SceneCamera& camera, 
			float shadowBias, unsigned int width, unsigned int height);

		virtual void bindData(const Shader& shader, const std::string& name);
		virtual void bindMap(Shader& shader, const std::string& name);
		virtual void removeMap(Shader& shader);

		virtual void draw(const SceneCamera* const camera,
			std::function<void(const Shader&)> renderCall, const Shader& shader);

		//Set split planes (between cameras near and far clip plane)
		void setCascades(const SceneCamera& camera);

		virtual unsigned int getID() const;

	private:
		unsigned int ID;

		CascadedMap shadowMaps[MAPCOUNT];

		void computeLightTransforms(const SceneCamera& camera);
	};

}

#endif
