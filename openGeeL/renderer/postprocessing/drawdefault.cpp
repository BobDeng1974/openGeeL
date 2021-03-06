#define GLEW_STATIC
#include <glew.h>
#include <vec3.hpp>
#include "shader/rendershader.h"
#include "shader/shaderreader.h"
#include "primitives/screenquad.h"
#include "framebuffer/framebuffer.h"
#include "texturing/imagetexture.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "drawdefault.h"

using namespace glm;

namespace geeL {

	DefaultPostProcess::DefaultPostProcess(const Resolution& windowResolution, float exposure, TonemappingMethod method, bool adaptive)
		: PostProcessingEffectFS(defaultVertexPath, "shaders/postprocessing/drawdefault.frag", 
			StringReplacement("^#define TONEMAPPING_METHOD\\s+([0-9]+){1}\\s?",
				std::to_string((int)method), 1))
		, resolution(windowResolution)
		, customTexture(nullptr)
		, adaptiveExposure(adaptive) {
	
		noise = ImageTexture::create<ImageTexture>("resources/textures/noise.png", ColorType::Single);
		shader.setValue("exposure", exposure, Range<float>(0.f, 100.f));
	}

	DefaultPostProcess::DefaultPostProcess(const DefaultPostProcess& other) 
		: PostProcessingEffectFS(other)
		, resolution(other.resolution)
		, customTexture(other.customTexture) {

		noise = ImageTexture::create<ImageTexture>("resources/textures/noise.png", ColorType::Single);
		shader.setValue("exposure", other.getExposure(), Range<float>(0.f, 100.f));
	}

	

	DefaultPostProcess & DefaultPostProcess::operator=(const DefaultPostProcess& other) {
		if (&other != this) {
			DefaultPostProcess s(other);
			*this = std::move(s);
		}

		return *this;
	}


	void DefaultPostProcess::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		shader.ignoreOptimisations = true;
		shader.addMap(*noise, "noiseMap");
		shader.bind<glm::vec3>("noiseScale",
			vec3(float(resolution.getWidth()) / 255.f, float(resolution.getHeight()) / 255.f, 0.f));
	}

	void DefaultPostProcess::draw() {
		RenderTexture& image = provider->requestCurrentImage();
		bool useAutoExposure = (customTexture == nullptr) && adaptiveExposure;
		bool useLinearFiltering = provider->getRenderResolution() != resolution;

		//Mipmap given image to get average scene brightness at lowest mip level
		if (useAutoExposure) {
			TextureParameters& p = provider->getParameters(FilterMode::Trilinear, WrapMode::ClampEdge, AnisotropicFilter::None);
			image.attachParameters(p);
			image.mipmap();
		}
		//Sample given image with linear texture filtering since it uses a different resolution as the application.
		//Note: This step is not necessary with active adaptive exposure, since image is already sampled with trillinear
		//texture filtering there
		else if (useLinearFiltering) {
			TextureParameters& p = provider->getParameters(FilterMode::Linear, WrapMode::ClampEdge, AnisotropicFilter::None);
			image.attachParameters(p);
		}

		if(customTexture == nullptr)
			setImage(image);

		//Force resolution to ensure final image is properly alligned to window aspect
		Viewport::set(0, 0, resolution.getWidth(), resolution.getHeight());
			
		PostProcessingEffectFS::draw();

		if (useAutoExposure || useLinearFiltering) {
			//Reset texture parameters
			TextureParameters& p = provider->getDefaultParameters();
			image.attachParameters(p);
		}
	}

	void DefaultPostProcess::setCustomImage(const ITexture* const texture) {
		customTexture = texture;

		if (customTexture != nullptr)
			setImage(*customTexture);
	}


	float DefaultPostProcess::getExposure() const {
		return shader.getFloatValue("exposure");
	}

	void DefaultPostProcess::setExposure(float exposure) {
		shader.setValue("exposure", exposure, Range<float>(0.f, 100.f));
	}

	bool DefaultPostProcess::getAdaptiveExposure() const {
		return adaptiveExposure;
	}

	void DefaultPostProcess::setAdaptiveExposure(bool value) {
		if (adaptiveExposure != value) {
			adaptiveExposure = value;
			shader.bind<bool>("adaptiveExposure", value);
		}
	}

}