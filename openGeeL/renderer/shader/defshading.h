#ifndef DEFAULTSHADING_H
#define DEFAULTSHADING_H

#include <string>

namespace geeL {

	enum class ShaderTransformSpace {
		World,
		View
	};

	enum class ShadingMethod {
		Generic,
		Deferred,
		Forward,
		TransparentOD,
		TransparentOID,
		Other

	};

	ShadingMethod getShadingMethod(const std::string& name);

}

#endif
