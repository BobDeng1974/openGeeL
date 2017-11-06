#ifndef IMAGEBASEDLIGHTING_H
#define IMAGEBASEDLIGHTING_H

#include "scenerender.h"
#include "postprocessing/additiveeffect.h"

namespace geeL {

	class RenderScene;

	class ImageBasedLighting : public SceneRender, public AdditiveEffect {

	public:
		ImageBasedLighting(RenderScene& scene);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void draw();
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		virtual std::string toString() const;

	protected:
		virtual void bindValues();

	private:
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

	};


	inline std::string ImageBasedLighting::toString() const {
		return "IBL";
	}


}

#endif
