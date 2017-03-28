#ifndef LIGHTBINDING_H
#define LIGHTBINDING_H

#include <string>

namespace geeL {

	class PointLight;
	class DirectionalLight;
	class SpotLight;


	//Helper structure for indexing lights. Primarily used by light manager
	class LightBinding {

	public:
		size_t index;

		LightBinding(size_t index, std::string baseName) : index(index), base(baseName) {}

		std::string getName() const {
			return base + "[" + std::to_string(index) + "].";
		}

		std::string getName(size_t index) const {
			return base + "[" + std::to_string(index) + "].";
		}

	private:
		std::string base;

	};


	class SLightBinding : public LightBinding {

	public:
		SpotLight* light;

		SLightBinding(SpotLight* light, size_t index, std::string baseName)
			: LightBinding(index, baseName), light(light) {}

		bool operator== (const SLightBinding &rhs) {
			return light == rhs.light;
		}
	};


	class DLightBinding : public LightBinding {

	public:
		DirectionalLight* light;

		DLightBinding(DirectionalLight* light, size_t index, std::string baseName)
			: LightBinding(index, baseName), light(light) {}

		bool operator== (const DLightBinding &rhs) {
			return light == rhs.light;
		}
	};


	class PLightBinding : public LightBinding {

	public:
		PointLight* light;

		PLightBinding(PointLight* light, size_t index, std::string baseName)
			: LightBinding(index, baseName), light(light) {}

		bool operator== (const PLightBinding &rhs) {
			return light == rhs.light;
		}
	};


}

#endif
