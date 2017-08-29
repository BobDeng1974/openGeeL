#ifndef RAYMARCH_H
#define RAYMARCH_H

#include "scenerender.h"

namespace geeL {

	class RayMarcher : public SceneRender, public PostProcessingEffectFS {

	public:
		RayMarcher(RenderScene& scene);

		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution);
		virtual void draw();

	protected:
		virtual void bindValues();

	};

}

#endif
