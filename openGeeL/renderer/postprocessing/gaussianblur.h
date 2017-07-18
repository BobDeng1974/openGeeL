#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "../framebuffer/colorbuffer.h"
#include "../utility/worldinformation.h"
#include "../utility/linearkernel.h"
#include "postprocessing.h"

namespace geeL {

	class RenderTexture;
	class SobelFilter;
	class Texture;


	enum class KernelSize {
		Small = 1,
		Medium = 2,
		Large = 3,
		Huge = 4
	};


	//Two pass gaussian blur
	class GaussianBlurBase : public PostProcessingEffect {

	public:
		~GaussianBlurBase();

		virtual void setImageBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution);

		std::vector<float> computeKernel(float sigma) const;

		float getSigma() const;
		void  setSigma(float value);

	protected:
		unsigned int currBuffer;
		
		GaussianBlurBase(float sigma = 1.3f);
		GaussianBlurBase(std::string shaderPath, float sigma = 1.3f);

		virtual void bindValues();
		void setKernelsize(unsigned int size);

	private:
		float sigma;
		unsigned int kernelSize = 5;

		LinearKernel linearKernel;
		const Texture* mainBuffer;
		ColorBuffer tempBuffer;
		RenderTexture* tempTexture = nullptr;
		ShaderLocation horLocation;

		void bindKernel() const;
		void updateKernel();

	};


	class GaussianBlur : public GaussianBlurBase {

	public:
		GaussianBlur(KernelSize kernelSize = KernelSize::Small, float sigma = 1.3f);

	};


	//Two pass gaussian blur that blurs depending on color differences and scaled with given factor
	class BilateralFilter : public GaussianBlurBase {

	public:
		BilateralFilter(float sigma = 1.3f, float factor = 0.5f);

		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution);

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
	class SobelBlur : public GaussianBlurBase, public WorldMapRequester {

	public:
		SobelBlur(SobelFilter& sobel, float sigma = 1.5f, bool depth = true);
		~SobelBlur();

		virtual void setImageBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution);

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		float getScale() const;
		void  setScale(float value);

	protected:
		virtual void bindValues();

	private:
		bool depth;
		SobelFilter& sobel;
		ColorBuffer sobelBuffer;
		RenderTexture* sobelTexture = nullptr;

	};
}

#endif
