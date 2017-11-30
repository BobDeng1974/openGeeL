#ifndef ENVIRONMENTMAP_H
#define ENVIRONMENTMAP_H

#include <string>
#include "texture.h"

namespace geeL {

	//2D equirectangular map of an 3D environment with HDR
	class EnvironmentMap : public Texture2D {

	public:
		EnvironmentMap();
		EnvironmentMap(const std::string& fileName);

	private:
		struct ImageContainer {
			int width, height, nrComponents;
			float* image;

			ImageContainer(const std::string& fileName);
			~ImageContainer();
		};

		EnvironmentMap(ImageContainer&& container);

		EnvironmentMap(const EnvironmentMap& map) = delete;
		EnvironmentMap& operator= (const EnvironmentMap& other) = delete;
		
	};

}

#endif

