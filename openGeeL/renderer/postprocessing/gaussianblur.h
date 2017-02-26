#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "../utility/framebuffer.h"
#include "../utility/worldrequester.h"
#include "postprocessing.h"

namespace geeL {

	//Two pass gaussian blur
	class GaussianBlur : public PostProcessingEffect {

	public:
		GaussianBlur(unsigned int strength = 1);
		
		void setKernel(float newKernel[5]);
		virtual void init(ScreenQuad& screen);

	protected:
		GaussianBlur(unsigned int strength, std::string shaderPath);

		virtual void bindValues();
		virtual void bindToScreen();

	private:
		unsigned int currBuffer;
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

		virtual WorldMaps requiredWorldMaps() const;
		virtual WorldMatrices requiredWorldMatrices() const;
		virtual WorldVectors requiredWorldVectors() const;
		virtual std::list<WorldMaps> requiredWorldMapsList() const;

		virtual void addWorldInformation(std::list<unsigned int> maps,
			std::list<const glm::mat4*> matrices, std::list<const glm::vec3*> vectors);

	protected:
		virtual void bindValues();

	};
}

#endif
