#ifndef LUACONFIGURATION_H
#define LUACONFIGURATION_H

#include <string>
#include <vec3.hpp>
#include "sol.hpp"

namespace geeL {

	//Configure and run scene from given lua file
	class LUAConfigurator {

	public:
		LUAConfigurator(const std::string& filePath);

		void run();

	private:
		sol::state state;

	};

}

#endif
