#include "defshading.h"

namespace geeL {



	ShadingMethod getShadingMethod(const std::string& name) {
		if (name == "Forward")
			return ShadingMethod::Forward;
		else if (name == "Hybrid")
			return ShadingMethod::Hybrid;
		else if (name == "Transparent")
			return ShadingMethod::Transparent;

		return ShadingMethod::Deferred;
	}

}