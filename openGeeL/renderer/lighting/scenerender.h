#ifndef SCENERENDER_H
#define SCENERENDER_H

#include "utility/worldinformation.h"
#include "postprocessing/postprocessing.h"

namespace geeL {

	class RenderScene;

	//Post effect for drawing entire scene content. E.g. for deferred lighting
	class SceneRender : public PostProcessingEffectFS, public WorldMapRequester, public CameraRequester {

	public:
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps) {}

	protected:
		SceneRender(RenderScene& scene, std::string fragment);
		SceneRender(RenderScene& scene, std::string vertex, std::string fragment);

		RenderScene& scene;

	};
}

#endif

