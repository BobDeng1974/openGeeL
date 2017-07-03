#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include <vector>
#include "../framebuffer/framebuffer.h"
#include "../utility/worldinformation.h"
#include "postprocessing.h"

namespace geeL {

	class SobelFilter;
	class Texture;


	//Two pass gaussian blur
	class GaussianBlur : public PostProcessingEffect {

	public:
		GaussianBlur(float sigma = 1.3f);
		
		virtual void setBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		std::vector<float> computeKernel(float sigma) const;

		float getSigma() const;
		void  setSigma(float value);

		unsigned int getStrength() const;
		void setStrength(unsigned int value);

	protected:
		unsigned int currBuffer;
		
		GaussianBlur(std::string shaderPath, float sigma = 1.3f);

		virtual void bindValues();
		void setKernelsize(unsigned int size);

	private:
		float sigma;
		unsigned int kernelSize = 5;
		unsigned int amount;

		const Texture* mainBuffer;
		std::vector<float> kernel;
		ColorBuffer frameBuffers[2];
		ShaderLocation horLocation;

	};


	//Two pass gaussian blur that blurs depending on color differences and scaled with given factor
	class BilateralFilter : public GaussianBlur {

	public:
		BilateralFilter(float sigma = 1.3f, float factor = 0.5f);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		float getSigma() const;
		void  setSigma(float value);

	protected:
		BilateralFilter(std::string shaderPath, float sigma = 1.3f, float factor = 0.5f);

	private:
		float sigma2;

	};


	//Two pass gaussian blur that blurs depending on depth differences and scaled with given factor
	class BilateralDepthFilter : public BilateralFilter, public WorldMapRequester {

	public:
		BilateralDepthFilter(float sigma = 1.3f, float factor = 0.5f);

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	};


	//Two pass gaussian blur that blurs depending sobel edge detection
	class SobelBlur : public GaussianBlur, public WorldMapRequester {

	public:
		SobelBlur(SobelFilter& sobel, float sigma = 1.5f);

		virtual void setBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		float getScale() const;
		void  setScale(float value);

	protected:
		virtual void bindValues();

	private:
		SobelFilter& sobel;
		ColorBuffer sobelBuffer;

	};
}

#endif
