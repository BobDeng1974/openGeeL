#include "defshading.h"

namespace geeL {



	ShadingMethod getShadingMethod(const std::string& name) {
		if (name == "Generic")
			return ShadingMethod::Generic;
		else if (name == "Forward")
			return ShadingMethod::Forward;
		else if (name == "TransparentOD")
			return ShadingMethod::TransparentOD;
		else if (name == "TransparentOID")
			return ShadingMethod::TransparentOID;

		return ShadingMethod::Deferred;
	}

}