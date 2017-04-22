#ifndef DEFERREDLIGHTING_H
#define DEFERREDLIGHTING_H

#include "postprocessing.h"
#include "../utility/worldinformation.h"

namespace geeL {

	class RenderScene;

	class DeferredLighting : public PostProcessingEffect, public WorldMapRequester, public CameraRequester {

	public:
		DeferredLighting(RenderScene& scene);

		virtual void setBuffer(unsigned int buffer) {}
		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);
		virtual void updateSkybox(Skybox& skybox);

	protected:
		virtual void bindValues();

	private:
		RenderScene& scene;
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

	};
}

#endif
