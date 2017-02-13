#ifndef WORLDPOSTPROCESSINGEFFECT_H
#define WORLDPOSTPROCESSINGEFFECT_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <list>
#include <string>
#include "postprocessing.h"

namespace geeL {

	enum class WorldMaps {
		None = 0,
		RenderedImage = 1,
		DiffuseRoughness = 2,
		PositionDepth = 4,
		NormalMetallic = 8,
		Specular = 16
	};

	enum class WorldMatrices {
		None = 0,
		View = 1,
		Projection = 2
	};

	enum class WorldVectors {
		None = 0,
		CameraPosition = 1,
		CameraDirection = 2,
		OriginView = 4
	};


	//Special type of post processing effect that uses additional world information (like positions or normals)
	class WorldPostProcessingEffect : public PostProcessingEffect {

	public:
		WorldPostProcessingEffect(std::string fragmentPath);
		WorldPostProcessingEffect(std::string vertexPath, std::string fragmentPath);

		virtual WorldMaps requiredWorldMaps() const = 0;
		virtual WorldMatrices requiredWorldMatrices() const = 0;
		virtual WorldVectors requiredWorldVectors() const = 0;

		virtual std::list<WorldMaps> requiredWorldMapsList() const;
		virtual std::list<WorldMatrices> requiredWorldMatricesList() const;
		virtual std::list<WorldVectors> requiredWorldVectorsList() const;

		virtual void addWorldInformation(std::list<unsigned int> maps, 
			std::list<glm::mat4> matrices,std::list<glm::vec3> vectors) = 0;

	};


	inline WorldMaps operator|(WorldMaps a, WorldMaps b) {
		return static_cast<WorldMaps>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline WorldMatrices operator|(WorldMatrices a, WorldMatrices b) {
		return static_cast<WorldMatrices>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline WorldVectors operator|(WorldVectors a, WorldVectors b) {
		return static_cast<WorldVectors>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline WorldMaps operator&(WorldMaps a, WorldMaps b) {
		return static_cast<WorldMaps>(static_cast<int>(a) & static_cast<int>(b));
	}

	inline WorldMatrices operator&(WorldMatrices a, WorldMatrices b) {
		return static_cast<WorldMatrices>(static_cast<int>(a) & static_cast<int>(b));
	}

	inline WorldVectors operator&(WorldVectors a, WorldVectors b) {
		return static_cast<WorldVectors>(static_cast<int>(a) & static_cast<int>(b));
	}
}

#endif