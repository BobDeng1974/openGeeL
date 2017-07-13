#ifndef IMAGEBASEDLIGHTING_H
#define IMAGEBASEDLIGHTING_H

#include "scenerender.h"

namespace geeL {

	class RenderScene;

	class ImageBasedLighting : public SceneRender {

	public:
		ImageBasedLighting(RenderScene& scene);

		virtual void init(ScreenQuad& screen, const ColorBuffer& buffer);
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	protected:
		virtual void bindValues();

	private:
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

	};
}

#endif
