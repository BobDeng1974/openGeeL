#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "../framebuffer/framebuffer.h"
#include "../utility/worldinformation.h"
#include "postprocessing.h"

namespace geeL {

	class SobelFilter;


	//Two pass gaussian blur
	class GaussianBlur : public PostProcessingEffect {

	public:
		GaussianBlur(unsigned int strength = 1);
		
		void setKernel(float newKernel[5]);
		virtual void setBuffer(unsigned int buffer);
		
		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

	protected:
		unsigned int currBuffer;

		GaussianBlur(unsigned int strength, std::string shaderPath);

		virtual void bindValues();

	private:
		unsigned int amount;
		unsigned int maxAmount = 10;
		unsigned int mainBuffer;
		float kernel[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
		ColorBuffer frameBuffers[2];
		ShaderLocation horLocation;
	};


	//Two pass gaussian blur that blurs depending on color differences and scaled with given factor
	class BilateralFilter : public GaussianBlur {

	public:
		BilateralFilter(unsigned int strength = 1, float sigma = 0.5f);

		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

	protected:
		BilateralFilter(std::string shaderPath, unsigned int strength = 1, float sigma = 0.5f);

	private:
		float sigma;

	};


	//Two pass gaussian blur that blurs depending on depth differences and scaled with given factor
	class BilateralDepthFilter : public BilateralFilter, public WorldMapRequester {

	public:
		BilateralDepthFilter(unsigned int strength = 1, float sigma = 0.5f);

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	};


	//Two pass gaussian blur that blurs depending sobel edge detection
	class SobelBlur : public GaussianBlur, public WorldMapRequester {

	public:
		SobelBlur(SobelFilter& sobel, unsigned int strength = 1);

		virtual void setBuffer(unsigned int buffer);
		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	protected:
		virtual void bindValues();

	private:
		SobelFilter& sobel;
		ColorBuffer sobelBuffer;

	};
}

#endif
