#ifndef LIGHTBINDING_H
#define LIGHTBINDING_H

#include <string>

namespace geeL {

	class Light;
	class PointLight;
	class DirectionalLight;
	class SpotLight;


	//Helper structure for indexing lights. Primarily used by light manager
	class LightBinding {

	public:
		Light* light;
		size_t index;

		LightBinding() : light(nullptr) {}
		LightBinding(Light* light, size_t index, std::string baseName) : light(light), index(index), base(baseName) {}

		std::string getName() const {
			return base + "[" + std::to_string(index) + "].";
		}

		std::string getName(size_t index) const {
			return base + "[" + std::to_string(index) + "].";
		}

		bool operator== (const LightBinding &rhs) {
			return light != nullptr && rhs.light != nullptr &&light == rhs.light;
		}

	private:
		std::string base;

	};



}

#endif
