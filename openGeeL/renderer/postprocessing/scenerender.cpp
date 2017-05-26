#include "../scene.h"
#include "scenerender.h"

namespace geeL {

	SceneRender::SceneRender(RenderScene& scene, std::string fragment)
		: scene(scene), PostProcessingEffect(fragment) {

		scene.addRequester(*this);
	}

	SceneRender::SceneRender(RenderScene& scene, std::string vertex, std::string fragment)
		: scene(scene), PostProcessingEffect(vertex, fragment) {
	
		scene.addRequester(*this);
	}


}