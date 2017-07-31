#include "renderscene.h"
#include "scenerender.h"

namespace geeL {

	SceneRender::SceneRender(RenderScene& scene, std::string fragment)
		: scene(scene), PostProcessingEffectFS(fragment) {

		scene.addRequester(*this);
	}

	SceneRender::SceneRender(RenderScene& scene, std::string vertex, std::string fragment)
		: scene(scene), PostProcessingEffectFS(vertex, fragment) {
	
		scene.addRequester(*this);
	}


}