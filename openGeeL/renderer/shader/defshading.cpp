#include "defshading.h"

namespace geeL {



	ShadingMethod getShadingMethod(const std::string& name) {
		if (name == "Generic")
			return ShadingMethod::Generic;
		else if (name == "Forward")
			return ShadingMethod::Forward;
		else if (name == "Transparent")
			return ShadingMethod::Transparent;

		return ShadingMethod::Deferred;
	}

}