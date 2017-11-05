#ifndef IMAGEBASEDLIGHTING_H
#define IMAGEBASEDLIGHTING_H

#include "scenerender.h"
#include "postprocessing/additiveeffect.h"

namespace geeL {

	class RenderScene;

	class ImageBasedLighting : public SceneRender, public AdditiveEffect {

	public:
		ImageBasedLighting(RenderScene& scene);

		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution);
		virtual void draw();
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	protected:
		virtual void bindValues();

	private:
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

	};

}

#endif
