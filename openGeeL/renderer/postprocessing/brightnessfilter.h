#ifndef BRIGHTNESSFILTER_H
#define BRIGHTNESSFILTER_H

#include "postprocessing.h"

namespace geeL {

	//Filter effect that filters bright colors / lights in image depending on given scatter
	class BrightnessFilter : public PostProcessingEffect {

	public:
		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer) = 0;

	protected:
		BrightnessFilter(const std::string& path);

	};


	
	class BrightnessFilterCutoff : public BrightnessFilter {

	public:
		float scatter;

		BrightnessFilterCutoff(float scatter = 0.6f);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		void setScatter(float scatter);
		float getScatter() const;

	private:
		ShaderLocation scatterLocation;

	};



	class BrightnessFilterSmooth : public BrightnessFilter {

	public:
		BrightnessFilterSmooth(float bias = 1.f, float scale = 0.001f);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		float getBias() const;
		float getScale() const;

		void setBias(float value);
		void setScale(float value);

	private:
		float bias, scale;

	};


}

#endif

