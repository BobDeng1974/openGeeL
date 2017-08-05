#ifndef MAPTYPE_H
#define MAPTYPE_H

#include <algorithm>
#include <string>

namespace geeL {

	enum class MapType {
		Diffuse,
		Specular,
		Normal,
		Metallic

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
			case MapType::Normal:
				return "normal";
		}

		return "";
	}

	inline MapType MapTypeConversion::getMapFromString(const std::string& name) {
		std::string type = name;
		std::transform(type.begin(), type.end(), type.begin(), ::tolower);

		if (type == "diffuse") return MapType::Diffuse;
		if (type == "specular") return MapType::Specular;
		if (type == "normal") return MapType::Normal;

		throw "Given name can't be converted into a map type\n";
	}

}


#endif
