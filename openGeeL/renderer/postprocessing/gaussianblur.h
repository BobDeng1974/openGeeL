#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "utility/worldinformation.h"
#include "utility/linearkernel.h"
#include "postprocessing.h"

namespace geeL {

	class RenderTexture;
	class SobelFilter;
	class ITexture;


	//Two pass gaussian blur
	class GaussianBlur : public PostProcessingEffectFS {

	public:
		GaussianBlur(float sigma = 1.3f, unsigned int kernelSize = 5);
		virtual ~GaussianBlur();

		virtual void setImage(const ITexture& texture);
		virtual void init(const PostProcessingParameter& parameter);

		static std::vector<float> computeKernel(float sigma, unsigned int kernelSize, bool halvedDistribution = false);
		std::vector<float> computeKernel(float sigma) const;

		float getSigma() const;
		virtual void setSigma(float value);

		virtual std::string toString() const;

	protected:
		GaussianBlur(std::string shaderPath, float sigma = 1.3f,
			unsigned int kernelSize = 5);

		virtual void drawSubImages();

	private:
		float sigma;
		unsigned int kernelSize;

		LinearKernel linearKernel;
		const ITexture* mainBuffer;
		RenderTexture* tempTexture = nullptr;
		ShaderLocation horLocation;

		void setKernelsize(unsigned int size);
		void updateKernel();

	};


	class SeparatedGaussian : public GaussianBlur {

	public:
		SeparatedGaussian(float sigma = 1.3f, unsigned int kernelSize = 7, float falloff = 0.f);

		virtual void init(const PostProcessingParameter& parameter);

		float getSigmaR() const;
		float getSigmaG() const ;
		float getSigmaB() const;
		float getFalloff() const;

		virtual void setSigma(float value);
		void setSigmaR(float value);
		void setSigmaG(float value);
		void setSigmaB(float value);
		void setFalloff(float value);

		virtual std::string toString() const;

	private:
		float sigmaR, sigmaG, sigmaB, falloff;
		LinearKernel kernelR, kernelG, kernelB;

	};


	//Two pass gaussian blur that blurs depending on color differences and scaled with given factor
	class BilateralFilter : public GaussianBlur {

	public:
		BilateralFilter(float sigma = 1.3f, unsigned int kernelSize = 7, float factor = 0.5f);

		virtual void init(const PostProcessingParameter& parameter);

		float getFactor() const;
		void  setFactor(float value);

		virtual std::string toString() const;

	protected:
		BilateralFilter(std::string shaderPath, float sigma = 1.3f, 
			unsigned int kernelSize = 7, float factor = 0.5f);

	private:
		float sigma2;

	};


	//Two pass gaussian blur that blurs depending on depth differences and scaled with given factor
	class BilateralDepthFilter : public BilateralFilter {

	public:
		BilateralDepthFilter(float sigma = 1.3f, unsigned int kernelSize = 7, float factor = 0.5f);

	};


	//Two pass gaussian blur that blurs depending sobel edge detection
	class SobelBlur : public GaussianBlur {

	public:
		SobelBlur(SobelFilter& sobel, float sigma = 1.5f, unsigned int kernelSize = 7, bool depth = true);
		virtual ~SobelBlur();

		virtual void setImage(const ITexture& texture);
		virtual void init(const PostProcessingParameter& parameter);

		float getScale() const;
		void  setScale(float value);

		virtual std::string toString() const;

	protected:
		virtual void drawSubImages();

	private:
		bool depth;
		SobelFilter& sobel;
		RenderTexture* sobelTexture = nullptr;

	};


	inline std::string GaussianBlur::toString() const {
		return "Gaussian Blur";
	}

	inline std::string SeparatedGaussian::toString() const {
		return "Separated Gaussian Blur";
	}

	inline std::string BilateralFilter::toString() const {
		return "Bilateral Filter";
	}

	inline std::string SobelBlur::toString() const {
		return "Sobel Blur";
	}

}

#endif
