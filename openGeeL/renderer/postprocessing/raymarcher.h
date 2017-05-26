#ifndef RAYMARCH_H
#define RAYMARCH_H

#include "scenerender.h"

namespace geeL {

	class RayMarcher : public SceneRender {

	public:
		RayMarcher(RenderScene& scene);

	protected:
		virtual void bindValues();

	};

}

#endif
