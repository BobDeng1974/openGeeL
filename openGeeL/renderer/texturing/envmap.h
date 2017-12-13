#ifndef ENVIRONMENTMAP_H
#define ENVIRONMENTMAP_H

#include <string>
#include "imagetexture.h"

namespace geeL {

	//2D equirectangular map of an 3D environment with HDR
	class EnvironmentMap : public ImageTexture {

	public:
		EnvironmentMap(const std::string& fileName);

	private:
		struct ImageContainer {
			int width, height, nrComponents;
			float* image;

			ImageContainer(const std::string& fileName);
		};

		EnvironmentMap(ImageContainer&& container);

		EnvironmentMap(const EnvironmentMap& map) = delete;
		EnvironmentMap& operator= (const EnvironmentMap& other) = delete;
		
	};

}

#endif

