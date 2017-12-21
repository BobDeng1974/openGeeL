#ifndef IMAGEBASEDLIGHTING_H
#define IMAGEBASEDLIGHTING_H

#include "scenerender.h"
#include "postprocessing/additiveeffect.h"

namespace geeL {

	class MaterialFactory;
	class RenderScene;

	class ImageBasedLighting : public SceneRender, public AdditiveEffect {

	public:
		bool useSSAO = true;

		ImageBasedLighting(RenderScene& scene);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();
		virtual void draw();
		virtual void fill();

		virtual void setActive(bool value);

		virtual std::string toString() const;

	private:
		MaterialFactory* factory;
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

		void activateTransparentIBL(bool activate);

	};


	inline std::string ImageBasedLighting::toString() const {
		return "IBL";
	}

	
}

#endif
