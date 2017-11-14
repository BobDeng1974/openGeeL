#ifndef RAYMARCH_H
#define RAYMARCH_H

#include "scenerender.h"

namespace geeL {

	class RayMarcher : public SceneRender, public PostProcessingEffectFS {

	public:
		RayMarcher(RenderScene& scene);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();
		virtual void draw();
		virtual void fill();

	};

}

#endif
