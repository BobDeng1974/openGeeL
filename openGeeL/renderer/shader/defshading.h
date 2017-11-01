#ifndef DEFAULTSHADING_H
#define DEFAULTSHADING_H

namespace geeL {

	enum class ShaderTransformSpace {
		World,
		View
	};

	enum class ShadingMethod {
		Generic,
		GenericSkinned,
		Deferred,
		DeferredSkinned,
		Forward,
		ForwardSkinned,
		Other

	};

}

#endif
