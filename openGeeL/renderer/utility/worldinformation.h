#ifndef WORLDINFORMATION_H
#define WORLDINFORMATION_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <map>

namespace geeL {

	enum class WorldMaps {
		None = 0,
		DiffuseRoughness = 1,
		PositionDepth = 2,
		NormalMetallic = 4,
	};

	enum class WorldMatrices {
		None = 0,
		View = 1,
		Projection = 2,
		InverseView = 3
	};

	enum class WorldVectors {
		None = 0,
		CameraPosition = 1,
		CameraDirection = 2,
		OriginView = 4
	};


	//Object that can request and is dependend on world information 
	class WorldInformationRequester {

	public:
		virtual void addWorldInformation(std::map<WorldMaps, unsigned int> maps,
			std::map<WorldMatrices, const glm::mat4*> matrices,
			std::map<WorldVectors, const glm::vec3*> vectors) = 0;

	};


	//Object that can provide world information to linked requesters
	class WorldInformationProvider {

	public:
		virtual void addRequester(WorldInformationRequester& requester) = 0;
		virtual void linkInformation() const = 0;

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

