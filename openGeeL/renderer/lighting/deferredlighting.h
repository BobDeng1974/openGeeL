#ifndef DEFERREDLIGHTING_H
#define DEFERREDLIGHTING_H

#include "scenerender.h"

namespace geeL {

	class RenderScene;

	class DeferredLighting : public SceneRender, public PostProcessingEffectFS {

	public:
		DeferredLighting(RenderScene& scene);

		virtual void setBuffer(unsigned int buffer) {}
		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();
		virtual void draw();

	private:
		ShaderLocation projectionLocation;
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

	};


	class TiledDeferredLighting : public SceneRender, public PostProcessingEffectCS {

	public:
		TiledDeferredLighting(RenderScene& scene);

		virtual void setBuffer(unsigned int buffer) {}
		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();
		virtual void draw();

	private:
		ShaderLocation projectionLocation;
		ShaderLocation invViewLocation;
		ShaderLocation originLocation;

	};



}

#endif
