#ifndef SCENERENDER_H
#define SCENERENDER_H

#include "utility/worldinformation.h"
#include "postprocessing/postprocessing.h"
#include "renderscene.h"

namespace geeL {

	class RenderScene;

	//Post effect for drawing entire scene content. E.g. for deferred lighting
	class SceneRender : public WorldMapRequester, public CameraRequester {

	public:
		virtual void init(const PostProcessingParameter& parameter) = 0;
		virtual void bindValues() = 0;
		virtual void draw() = 0;

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps) {}

	protected:
		SceneRender(RenderScene& scene);

		RenderScene& scene;

	};


	inline SceneRender::SceneRender(RenderScene& scene) : scene(scene) {
		scene.addRequester(*this);
	}

}

#endif

