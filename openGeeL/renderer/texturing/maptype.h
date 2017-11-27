#ifndef MAPTYPE_H
#define MAPTYPE_H

#include <algorithm>
#include <string>

namespace geeL {

	enum class MapType {
		Diffuse,
		Specular,
		Roughness,
		Normal,
		Metallic,
		Alpha,
		Emission,
		Occlusion

	};


	class MapTypeConversion {

	public:
		static std::string getTypeAsString(MapType type);
		static MapType getMapFromString(const std::string& name);

	};


	inline std::string MapTypeConversion::getTypeAsString(MapType type) {
		switch (type) {
			case MapType::Diffuse:
				return "diffuse";
			case MapType::Specular:
				return "specular";
			case MapType::Roughness:
				return "roughness";
			case MapType::Metallic:
				return "metallic";
			case MapType::Normal:
				return "normal";
			case MapType::Alpha:
				return "alpha";
			case MapType::Emission:
				return "emission";
			case MapType::Occlusion:
				return "occlusion";
		}

		return "";
	}

	inline MapType MapTypeConversion::getMapFromString(const std::string& name) {
		std::string type = name;
		std::transform(type.begin(), type.end(), type.begin(), ::tolower);

		if (type == "diffuse") return MapType::Diffuse;
		if (type == "specular") return MapType::Specular;
		if (type == "roughness") return MapType::Roughness;
		if (type == "metallic") return MapType::Metallic;
		if (type == "normal") return MapType::Normal;
		if (type == "alpha") return MapType::Alpha;
		if (type == "emission") return MapType::Emission;
		if (type == "occlusion") return MapType::Occlusion;

		throw "Given name can't be converted into a map type\n";
	}

}


#endif
