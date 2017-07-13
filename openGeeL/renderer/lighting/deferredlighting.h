#ifndef DEFERREDLIGHTING_H
#define DEFERREDLIGHTING_H

#include "scenerender.h"

namespace geeL {

	class RenderScene;

	class DeferredLighting : public SceneRender {

	public:
		DeferredLighting(RenderScene& scene);

		virtual void setImageBuffer(unsigned int buffer) {}
		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	protected:
		virtual void bindValues();

	private:
		ShaderLocation projectionLocation;
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

	};
}

#endif
