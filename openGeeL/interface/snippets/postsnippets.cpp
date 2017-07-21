#include "../guiwrapper.h"
#include "../../renderer/lighting/conetracer.h"
#include "../../renderer/postprocessing/drawdefault.h"
#include "../../renderer/postprocessing/blurredeffect.h"
#include "../../renderer/postprocessing/colorcorrection.h"
#include "../../renderer/postprocessing/bloom.h"
#include "../../renderer/postprocessing/dof.h"
#include "../../renderer/postprocessing/godray.h"
#include "../../renderer/postprocessing/lensflare.h"
#include "../../renderer/postprocessing/ssao.h"
#include "../../renderer/postprocessing/ssrr.h"
#include "../../renderer/postprocessing/fxaa.h"
#include "../../renderer/postprocessing/volumetriclight.h"
#include "postsnippets.h"
#include <iostream>

namespace geeL {

	DefaultSnippet::DefaultSnippet(DefaultPostProcess& def) : def(def) {}

	BlurredEffectSnippet::BlurredEffectSnippet(BlurredPostEffect& effect, GUISnippet& effectSnippet) 
		: effect(effect), effectSnippet(effectSnippet), blurSnippet(nullptr) {}

	ColorCorrectionSnippet::ColorCorrectionSnippet(ColorCorrection& color) : color(color) {}

	BloomSnippet::BloomSnippet(Bloom& bloom) : bloom(bloom) {}

	BrightnessFilterSnippet::BrightnessFilterSnippet(BrightnessFilterSmooth & filter) : filter(filter) {}

	DepthOfFieldBlurredSnippet::DepthOfFieldBlurredSnippet(DepthOfFieldBlurred& dof) : dof(dof) {}

	FXAASnippet::FXAASnippet(FXAA& fxaa) : fxaa(fxaa) {}

	GodRaySnippet::GodRaySnippet(GodRay& ray) : ray(ray) {}

	LensFlareSnippet::LensFlareSnippet(LensFlare& flare) : flare(flare) {}

	VolumetricLightSnippet::VolumetricLightSnippet(VolumetricLight& light) : light(light) {}

	SSAOSnippet::SSAOSnippet(SSAO& ssao) : ssao(ssao) {}

	SSRRSnippet::SSRRSnippet(SSRR& ssrr) : ssrr(ssrr) {}

	ConeTracerSnippet::ConeTracerSnippet(VoxelConeTracer & tracer) : tracer(tracer) {}

	PostGroupSnippet::PostGroupSnippet(std::list<PostEffectSnippet*>& snippets) : snippets(snippets) {
		if (snippets.size() == 0)
			throw "Post group needs at least one effect to work\n";
	}

	void PostGroupSnippet::draw(GUIContext * context) {
		for (auto it = next(snippets.begin()); it != snippets.end(); it++) {
			PostEffectSnippet& snippet = **it;

			GUISnippets::drawTreeNode(context, snippet.toString(), false, [this, &snippet](GUIContext* context) {
				snippet.draw(context);
			});
		}
	}

	std::string PostGroupSnippet::toString() const {
		return snippets.front()->toString();
	}

	void DefaultSnippet::draw(GUIContext* context) {
		float defExposure = def.getExposure();
		float exposure = GUISnippets::drawBarFloat(context, defExposure, 0.f, 25.f, 0.1f, "Exposure");
		if (exposure != defExposure)
			def.setExposure(exposure);
	}

	std::string DefaultSnippet::toString() const {
		return "Default";
	}


	void BlurredEffectSnippet::draw(GUIContext* context) {
		
		const ResolutionScale& oldResolution = effect.getEffectResolution();
		float effectResolution = GUISnippets::drawBarFloat(context, 
			oldResolution.get(), 0.f, 1.f, 0.001f, "Resolution");

		if (effectResolution != oldResolution.get())
			effect.resizeEffectResolution(effectResolution);

		effectSnippet.draw(context);

		if (blurSnippet != nullptr) {
			GUISnippets::drawTreeNode(context, "Blur (" + blurSnippet->toString() + ")", true, 
				[this](GUIContext* context) {
				
				blurSnippet->draw(context);
			});
		}
	}

	std::string BlurredEffectSnippet::toString() const {
		return "Blurred " + effectSnippet.toString();
	}

	void BlurredEffectSnippet::setBlurSnippet(GUISnippet& blurSnippet) {
		this->blurSnippet = &blurSnippet;
	}


	void ColorCorrectionSnippet::draw(GUIContext* context) {

		float r = GUISnippets::drawBarFloat(context, color.getRed(), 0.f, 1.f, 0.001f, "Red");
		color.setRed(r);

		float g = GUISnippets::drawBarFloat(context, color.getGreen(), 0.f, 1.f, 0.001f, "Green");
		color.setGreen(g);

		float b = GUISnippets::drawBarFloat(context, color.getBlue(), 0.f, 1.f, 0.001f, "Blue");
		color.setBlue(b);

		float h = GUISnippets::drawBarFloat(context, color.getHue(), 0.f, 1.f, 0.001f, "Hue");
		color.setHue(h);

		float s = GUISnippets::drawBarFloat(context, color.getSaturation(), 0.f, 1.f, 0.001f, "Saturation");
		color.setSaturation(s);

		float v = GUISnippets::drawBarFloat(context, color.getBrightness(), 0.f, 1.f, 0.001f, "Brightness");
		color.setBrightness(v);


		GUISnippets::drawTreeNode(context, "Chromatic Aberration", true, [this](GUIContext* context) {
			const glm::vec2& direction = color.getDistortionDirection();
			const glm::vec3& distortion = color.getChromaticDistortion();

			float dr = GUISnippets::drawBarFloat(context, distortion.r, 0.f, 0.01f, 0.0001f, "Red");
			float dg = GUISnippets::drawBarFloat(context, distortion.g, 0.f, 0.01f, 0.0001f, "Green");
			float db = GUISnippets::drawBarFloat(context, distortion.b, 0.f, 0.01f, 0.0001f, "Blue");

			color.setChromaticDistortion(glm::vec3(dr, dg, db));

			float dx = GUISnippets::drawBarFloat(context, direction.x, -1.f, 1.f, 0.001f, "X");
			float dy = GUISnippets::drawBarFloat(context, direction.y, -1.f, 1.f, 0.001f, "Y");

			color.setDistortionDirection(glm::vec2(dx, dy));
		});
		
	}

	std::string ColorCorrectionSnippet::toString() const {
		return "Color Correction";
	}


	void BloomSnippet::draw(GUIContext* context) {
		const ResolutionScale& oldResolution = bloom.getEffectResolution();
		float effectResolution = GUISnippets::drawBarFloat(context,
			oldResolution.get(), 0.f, 1.f, 0.001f, "Resolution");

		if (effectResolution != oldResolution.get())
			bloom.resizeEffectResolution(effectResolution);

		float scatter = GUISnippets::drawBarFloat(context, bloom.getScatter(), 0.f, 1.f, 0.005f, "Scatter");
		bloom.setScatter(scatter);
	}

	std::string BloomSnippet::toString() const {
		return "Bloom";
	}
	

	void BrightnessFilterSnippet::draw(GUIContext* context) {
		float bias = GUISnippets::drawBarFloat(context, filter.getBias(), 0.f, 3.f, 0.0001f, "Bias");
		filter.setBias(bias);

		float scale = GUISnippets::drawBarFloat(context, filter.getScale(), 0.f, 0.5f, 0.0001f, "Scale");
		filter.setScale(scale);

	}

	std::string BrightnessFilterSnippet::toString() const {
		return "Brightness Filter";
	}

	void DepthOfFieldBlurredSnippet::draw(GUIContext* context) {
		const ResolutionScale& oldResolution = dof.getBlurResolution();
		float blurResolution = GUISnippets::drawBarFloat(context,
			oldResolution.get(), 0.f, 1.f, 0.001f, "Resolution");

		if (blurResolution != oldResolution.get())
			dof.resizeBlurResolution(blurResolution);

		float aperture = GUISnippets::drawBarFloat(context, dof.getAperture(), 0.f, 100.f, 0.1f, "Aperture");
		dof.setAperture(aperture);

		GUISnippets::drawTreeNode(context, "Blur", true, [this](GUIContext* context) {
			DepthOfFieldBlur& blur = dof.getBlur();

			float sigma = GUISnippets::drawBarFloat(context, blur.getSigma(), 0.1f, 10.f, 0.001f, "Sigma");
			blur.setSigma(sigma);

			float threshold = GUISnippets::drawBarFloat(context, blur.getThreshold(), 0.f, 1.f, 0.01f, "Threshold");
			blur.setThreshold(threshold);
		});
	}

	std::string DepthOfFieldBlurredSnippet::toString() const {
		return "Depth of Field";
	}


	void FXAASnippet::draw(GUIContext* context) {
		float blurMin = GUISnippets::drawBarFloat(context, fxaa.getBlurMin(), 0.f, 0.1f, 0.0001f, "Blur Min");
		fxaa.setBlurMin(blurMin);

		float fxaaMul = GUISnippets::drawBarFloat(context, fxaa.getFXAAMul(), 0.f, 0.5f, 0.0001f, "FXAA Factor");
		fxaa.setFXAAMul(fxaaMul);

		float fxaaMin = GUISnippets::drawBarFloat(context, fxaa.getFXAAMin(), 0.f, 0.001f, 0.0001f, "FXAA Min");
		fxaa.setFXAAMin(fxaaMin);

		float fxaaClamp = GUISnippets::drawBarFloat(context, fxaa.getFXAAClamp(), 1.f, 32.f, 0.1f, "FXAA Clamp");
		fxaa.setFXAAClamp(fxaaClamp);
	}

	std::string FXAASnippet::toString() const {
		return "FXAA";
	}


	void GodRaySnippet::draw(GUIContext* context) {
		glm::vec3 position = GUISnippets::drawVector(context, ray.getLightPosition(), "", 100, 0.1f);
		ray.setLightPosition(position);

		int samples = GUISnippets::drawBarInteger(context, ray.getSampleCount(), 0, 25, 1, "Samples");
		ray.setSampleCount(samples);
	}

	std::string GodRaySnippet::toString() const {
		return "God Ray";
	}
	


	void LensFlareSnippet::draw(GUIContext * context) {
		float strength = GUISnippets::drawBarFloat(context, flare.getStrength(), 0.1f, 50.f, 0.001f, "Strength");
		flare.setStrength(strength);

		float scale = GUISnippets::drawBarFloat(context, flare.getScale(), 0.f, 0.5f, 0.001f, "Scale");
		flare.setScale(scale);

		float samples = GUISnippets::drawBarFloat(context, flare.getMaxSamples(), 0.f, 20.f, 0.1f, "Samples");
		flare.setMaxSamples(samples);

		GUISnippets::drawTreeNode(context, "Chromatic Aberration", true, [this](GUIContext* context) {
			const glm::vec3& distortion = flare.getDistortion();

			float dr = GUISnippets::drawBarFloat(context, distortion.r, 0.f, 0.1f, 0.0001f, "Red");
			float dg = GUISnippets::drawBarFloat(context, distortion.g, 0.f, 0.1f, 0.0001f, "Green");
			float db = GUISnippets::drawBarFloat(context, distortion.b, 0.f, 0.1f, 0.0001f, "Blue");

			flare.setDistortion(glm::vec3(dr, dg, db));
		});

	}

	std::string LensFlareSnippet::toString() const {
		return "Lens Flare";
	}


	void VolumetricLightSnippet::draw(GUIContext* context) {
		int samples = GUISnippets::drawBarInteger(context, light.getSampleCount(), 0, 500, 1, "Samples");
		light.setSampleCount(samples);

		float density = GUISnippets::drawBarFloat(context, light.getDensity(), 0.f, 5.f, 0.001f, "Density");
		light.setDensity(density);

		float distance = GUISnippets::drawBarFloat(context, light.getMinDistance(), 0.f, 50.f, 0.1f, "Distance");
		light.setMinDistance(distance);
	}
	
	std::string VolumetricLightSnippet::toString() const {
		return "Volumetric Light";
	}


	void SSAOSnippet::draw(GUIContext* context) {
		float radius = GUISnippets::drawBarFloat(context, ssao.getRadius(), 0.5f, 100.f, 0.1f, "Radius");
		ssao.setRadius(radius);
	}

	std::string SSAOSnippet::toString() const {
		return "SSAO";
	}


	void SSRRSnippet::draw(GUIContext* context) {
		
		//Lazy quick fix to allow fussy SSRR in GUI
		MultisampledSSRR* fussy = dynamic_cast<MultisampledSSRR*>(&ssrr);
		if (fussy != nullptr) {
			unsigned int samples = GUISnippets::drawBarInteger(context, fussy->getSampleCount(), 1, 100, 1, "Sample Count");
			fussy->setSampleCount(samples);
		}

		unsigned int steps = GUISnippets::drawBarInteger(context, ssrr.getStepCount(), 1, 200, 1, "Step Count");
		ssrr.setStepCount(steps);

		float size = GUISnippets::drawBarFloat(context, ssrr.getStepSize(), 0.01f, 1.f, 0.001f, "Step Size");
		ssrr.setStepSize(size);

		float gain = GUISnippets::drawBarFloat(context, ssrr.getStepSizeGain(), 1.f, 1.5f, 0.001f, "Step Gain");
		ssrr.setStepSizeGain(gain);
	}

	std::string SSRRSnippet::toString() const {
		return "SSRR";
	}
	

	void ConeTracerSnippet::draw(GUIContext* context) {
		unsigned int specLOD = GUISnippets::drawBarInteger(context, tracer.getSpecularLOD(), 1, 10, 1, "Specular LOD");
		tracer.setSpecularLOD(specLOD);

		unsigned int specSteps = GUISnippets::drawBarInteger(context, tracer.getSpecularSampleSize(), 1, 100, 1, "Specular Size");
		tracer.setSpecularSampleSize(specSteps);

		unsigned int diffLOD = GUISnippets::drawBarInteger(context, tracer.getDiffuseLOD(), 1, 10, 1, "Diffuse LOD");
		tracer.setDiffuseLOD(diffLOD);

		unsigned int diffSteps = GUISnippets::drawBarInteger(context, tracer.getDiffuseSampleSize(), 1, 50, 1, "Diffuse Size");
		tracer.setDiffuseSampleSize(diffSteps);
	}

	std::string ConeTracerSnippet::toString() const {
		return "Voxel Cone Tracer";
	}


	

	

}