#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "utility/worldinformation.h"
#include "utility/linearkernel.h"
#include "postprocessing.h"

namespace geeL {

	class RenderTexture;
	class SobelFilter;
	class ITexture;


	enum class KernelSize {
		Small = 1,
		Medium = 2,
		Large = 3,
		Huge = 4,
		Depth = 5
	};


	//Two pass gaussian blur
	class GaussianBlurBase : public PostProcessingEffectFS {

	public:
		virtual ~GaussianBlurBase();

		virtual void setImage(const ITexture& texture);
		virtual void init(const PostProcessingParameter& parameter);

		std::vector<float> computeKernel(float sigma) const;

		float getSigma() const;
		virtual void setSigma(float value);

		virtual std::string toString() const;

	protected:
		GaussianBlurBase(float sigma = 1.3f);
		GaussianBlurBase(std::string shaderPath, float sigma = 1.3f);

		virtual void drawSubImages();
		void setKernelsize(unsigned int size);

	private:
		float sigma;
		unsigned int kernelSize = 5;

		LinearKernel linearKernel;
		const ITexture* mainBuffer;
		RenderTexture* tempTexture = nullptr;
		ShaderLocation horLocation;

		void updateKernel();

	};


	class GaussianBlur : public GaussianBlurBase {

	public:
		GaussianBlur(KernelSize kernelSize = KernelSize::Small, float sigma = 1.3f);

	};


	class SeparatedGaussian : public GaussianBlurBase {

	public:
		SeparatedGaussian(float sigma = 1.3f, float falloff = 0.f);

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
	class BilateralFilter : public GaussianBlurBase {

	public:
		BilateralFilter(float sigma = 1.3f, float factor = 0.5f);

		virtual void init(const PostProcessingParameter& parameter);

		float getFactor() const;
		void  setFactor(float value);

		virtual std::string toString() const;

	protected:
		BilateralFilter(std::string shaderPath, float sigma = 1.3f, float factor = 0.5f);

	private:
		float sigma2;

	};


	//Two pass gaussian blur that blurs depending on depth differences and scaled with given factor
	class BilateralDepthFilter : public BilateralFilter {

	public:
		BilateralDepthFilter(float sigma = 1.3f, float factor = 0.5f);

	};


	//Two pass gaussian blur that blurs depending sobel edge detection
	class SobelBlur : public GaussianBlurBase {

	public:
		SobelBlur(SobelFilter& sobel, float sigma = 1.5f, bool depth = true);
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


	inline std::string GaussianBlurBase::toString() const {
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
