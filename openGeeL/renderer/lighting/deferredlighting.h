#ifndef DEFERREDLIGHTING_H
#define DEFERREDLIGHTING_H

#include "scenerender.h"

namespace geeL {

	class RenderScene;

	class DeferredLighting : public SceneRender {

	public:
		DeferredLighting(RenderScene& scene);

		virtual void setBuffer(unsigned int buffer) {}
		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution);

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
