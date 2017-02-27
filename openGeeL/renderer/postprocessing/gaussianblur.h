#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "../utility/framebuffer.h"
#include "../utility/worldinformation.h"
#include "postprocessing.h"

namespace geeL {

	class SobelFilter;


	//Two pass gaussian blur
	class GaussianBlur : public PostProcessingEffect {

	public:
		GaussianBlur(unsigned int strength = 1);
		
		void setKernel(float newKernel[5]);
		virtual void init(ScreenQuad& screen);

	protected:
		unsigned int currBuffer;

		GaussianBlur(unsigned int strength, std::string shaderPath);

		virtual void bindValues();
		virtual void bindToScreen();

	private:
		unsigned int amount;
		unsigned int maxAmount = 10;
		float kernel[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
		FrameBuffer frameBuffers[2];
	};


	//Two pass gaussian blur that blurs depending on color differences and scaled with given factor
	class BilateralFilter : public GaussianBlur {

	public:
		BilateralFilter(unsigned int strength = 1, float sigma = 0.5f);

	protected:
		BilateralFilter(std::string shaderPath, unsigned int strength = 1, float sigma = 0.5f);

		virtual void bindValues();

	private:
		float sigma;

	};


	//Two pass gaussian blur that blurs depending on depth differences and scaled with given factor
	class BilateralDepthFilter : public BilateralFilter, public WorldInformationRequester {

	public:
		BilateralDepthFilter(unsigned int strength = 1, float sigma = 0.5f);

		virtual void addWorldInformation(std::map<WorldMaps, unsigned int> maps,
			std::map<WorldMatrices, const glm::mat4*> matrices,
			std::map<WorldVectors, const glm::vec3*> vectors);

	protected:
		virtual void bindValues();

	};


	//Two pass gaussian blur that blurs depending sobel edge detection
	class SobelBlur : public GaussianBlur {

	public:
		SobelBlur(SobelFilter& sobel, unsigned int strength = 1);

		virtual void init(ScreenQuad& screen);

	protected:
		virtual void bindValues();
		virtual void bindToScreen();

	private:
		SobelFilter& sobel;
		FrameBuffer sobelBuffer;

	};
}

#endif
