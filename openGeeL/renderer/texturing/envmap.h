#ifndef ENVIRONMENTMAP_H
#define ENVIRONMENTMAP_H

#include <string>
#include "texture.h"

namespace geeL {

	//2D equirectangular map of an 3D environment with HDR
	class EnvironmentMap : public Texture2D {

	public:
		EnvironmentMap() : Texture2D(ColorType::RGB32) {}
		EnvironmentMap(const EnvironmentMap& map);
		EnvironmentMap(const std::string& fileName);
		
		virtual unsigned int getID() const;
		virtual void remove();

	private:
		unsigned int id;

	};

}

#endif

