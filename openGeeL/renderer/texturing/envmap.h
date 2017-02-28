#ifndef ENVIRONMENTMAP_H
#define ENVIRONMENTMAP_H

#include <string>

namespace geeL {

	//2D equirectangular map of an 3D environment with HDR
	class EnvironmentMap {

	public:
		EnvironmentMap(std::string fileName);
		
		const unsigned int GetID() const;

	private:
		unsigned int id;

	};

}

#endif

