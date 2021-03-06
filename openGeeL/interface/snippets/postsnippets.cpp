#include "guiwrapper.h"
#include "lighting/conetracer.h"
#include "shader/shader.h"
#include "postprocessing/drawdefault.h"
#include "postprocessing/blurredeffect.h"
#include "postprocessing/colorcorrection.h"
#include "postprocessing/bloom.h"
#include "postprocessing/dof.h"
#include "postprocessing/godray.h"
#include "postprocessing/lensflare.h"
#include "postprocessing/ssao.h"
#include "postprocessing/ssrr.h"
#include "postprocessing/fxaa.h"
#include "postprocessing/volumetriclight.h"
#include "postsnippets.h"
#include <iostream>

namespace geeL {

	PostEffectSnippet::PostEffectSnippet(PostProcessingEffect& baseEffect) : baseEffect(baseEffect) {}

	void PostEffectSnippet::draw(GUIContext* context) {

		int active = !baseEffect.isActive();
		nk_layout_row_dynamic(context, 30, 2);
		nk_checkbox_label(context, "Active", &active);
		baseEffect.setActive(!active);

		drawSimple(context);
	}

	PostProcessingEffect& PostEffectSnippet::getEffect() {
		return baseEffect;
	}

	std::string PostEffectSnippet::toString() const {
		return baseEffect.toString();
	}

	

	GenericPostSnippet::GenericPostSnippet(PostProcessingEffect& effect) 
		: PostEffectSnippet(effect), shader(effect.getShader()){

		//Read float and integer values from effects shader
		shader.iterateBindings([this](const ShaderBinding& binding) {
			const FloatBinding* floatBinding = dynamic_cast<const FloatBinding*>(&binding);
			if (floatBinding != nullptr)
				floatBindings.push_back(floatBinding);
			else {
				const IntegerBinding* intBinding = dynamic_cast<const IntegerBinding*>(&binding);
				if (intBinding != nullptr)
					integerBindings.push_back(intBinding);
			}
		});
	}

	void GenericPostSnippet::drawSimple(GUIContext * context) {
		float border = 200.f;
		for (auto it(floatBindings.begin()); it != floatBindings.end(); it++) {
			const FloatBinding& binding = **it;
			std::string name = binding.toString();

			const Range<float> range = binding.getRange();
			float min = (range.getMin() < -border) ? -border : range.getMin();
			float max = (range.getMax() > border) ? border : range.getMax();
			float step = (max - min) / 1000.f;

			float oldVal = shader.getFloatValue(name);
			float newVal = GUISnippets::drawBarFloat(context, oldVal, min, max, step, name);

			if (newVal != oldVal)
				shader.setValue(name, newVal);
		}

		for (auto it(integerBindings.begin()); it != integerBindings.end(); it++) {
			const IntegerBinding& binding = **it;
			std::string name = binding.toString();

			const Range<int> range = binding.getRange();
			int min = (range.getMin() < -border) ? -border : range.getMin();
			int max = (range.getMax() > border) ? border : range.getMax();
			int step = (max - min) / 1000.f;

			int oldVal = shader.getFloatValue(name);
			int newVal = GUISnippets::drawBarInteger(context, oldVal, min, max, 1, name);

			if (newVal != oldVal)
				shader.setValue(name, newVal);
		}
	}



	PostGroupSnippet::PostGroupSnippet(PostEffectSnippet& snippet) 
		: PostEffectSnippet(snippet), base(snippet) {}


	void PostGroupSnippet::drawSimple(GUIContext* context) {
		base.drawSimple(context);

		for (auto it = snippets.begin(); it != snippets.end(); it++) {
			PostEffectSnippet& snippet = **it;

			GUISnippets::drawTreeNode(context, snippet.toString(), false, [this, &snippet](GUIContext* context) {
				snippet.draw(context);
			});
		}
	}

	void PostGroupSnippet::add(PostEffectSnippet& snippet) {
		snippets.push_back(&snippet);
	}



	DefaultSnippet::DefaultSnippet(DefaultPostProcess& def) : PostEffectSnippet(def), def(def) {}

	void DefaultSnippet::drawSimple(GUIContext* context) {
		int adaptive = !def.getAdaptiveExposure();
		nk_checkbox_label(context, "Adaptive", &adaptive);
		def.setAdaptiveExposure(!adaptive);

		float defExposure = def.getExposure();
		float exposure = GUISnippets::drawBarFloatLogarithmic(context, defExposure, 0.f, 100.f, 0.1f, "Exposure");
		if (exposure != defExposure)
			def.setExposure(exposure);
	}



	BlurredEffectSnippet::BlurredEffectSnippet(BlurredPostEffect& effect, PostEffectSnippet& effectSnippet)
		: PostEffectSnippet(effect), effect(effect), effectSnippet(effectSnippet), blurSnippet(nullptr) {}

	void BlurredEffectSnippet::drawSimple(GUIContext* context) {
		const ResolutionPreset& oldResolution = effect.getEffectResolution();
		ResolutionPreset newResolution(GUISnippets::drawResolution(context, oldResolution));

		if (newResolution != oldResolution)
			effect.resizeEffectResolution(newResolution);

		effectSnippet.drawSimple(context);

		if (blurSnippet != nullptr) {
			GUISnippets::drawTreeNode(context, "Blur (" + blurSnippet->toString() + ")", true, 
				[this](GUIContext* context) {
				
				blurSnippet->draw(context);
			});
		}
	}

	void BlurredEffectSnippet::setBlurSnippet(GUISnippet& blurSnippet) {
		this->blurSnippet = &blurSnippet;
	}



	ColorCorrectionSnippet::ColorCorrectionSnippet(ColorCorrection& color) : PostEffectSnippet(color), color(color) {}

	void ColorCorrectionSnippet::drawSimple(GUIContext* context) {
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

		float c = GUISnippets::drawBarFloat(context, color.getContrast(), 0.f, 4.f, 0.001f, "Contrast");
		color.setContrast(c);


		float vi = GUISnippets::drawBarFloat(context, color.getVibrance(), -2.f, 2.f, 0.001f, "Vibrance");
		color.setVibrance(vi);


		GUISnippets::drawTreeNode(context, "Chromatic Aberration", true, [this](GUIContext* context) {
			const glm::vec2& direction = color.getDistortionDirection();
			const glm::vec3& distortion = color.getChromaticDistortion() * 1000.f;

			float dr = GUISnippets::drawBarFloat(context, distortion.r, -1.f, 1.f, 0.0001f, "Red");
			float dg = GUISnippets::drawBarFloat(context, distortion.g, -1.f, 1.f, 0.0001f, "Green");
			float db = GUISnippets::drawBarFloat(context, distortion.b, -1.f, 1.f, 0.0001f, "Blue");

			color.setChromaticDistortion(glm::vec3(dr, dg, db) * 0.001f);

			float dx = GUISnippets::drawBarFloat(context, direction.x, -1.f, 1.f, 0.001f, "X");
			float dy = GUISnippets::drawBarFloat(context, direction.y, -1.f, 1.f, 0.001f, "Y");

			color.setDistortionDirection(glm::vec2(dx, dy));
		});
		
	}



	BloomSnippet::BloomSnippet(Bloom& bloom) : PostEffectSnippet(bloom), bloom(bloom) {}

	void BloomSnippet::drawSimple(GUIContext* context) {
		const ResolutionPreset& oldResolution = bloom.getEffectResolution();
		ResolutionPreset newResolution(GUISnippets::drawResolution(context, oldResolution));

		if (newResolution != oldResolution)
			bloom.resizeEffectResolution(newResolution);

		float scatter = GUISnippets::drawBarFloatLogarithmic(context, bloom.getScatter(), 0.f, 5.f, 0.005f, "Scatter");
		bloom.setScatter(scatter);
	}



	BrightnessFilterSnippet::BrightnessFilterSnippet(BrightnessFilterSmooth & filter) : PostEffectSnippet(filter), filter(filter) {}

	void BrightnessFilterSnippet::drawSimple(GUIContext* context) {
		float bias = GUISnippets::drawBarFloat(context, filter.getBias(), 0.f, 3.f, 0.0001f, "Bias");
		filter.setBias(bias);

		float scale = GUISnippets::drawBarFloat(context, filter.getScale(), 0.f, 0.5f, 0.0001f, "Scale");
		filter.setScale(scale);

	}


	DepthOfFieldBlurredSnippet::DepthOfFieldBlurredSnippet(DepthOfFieldBlurred& dof) : PostEffectSnippet(dof), dof(dof) {}

	void DepthOfFieldBlurredSnippet::drawSimple(GUIContext* context) {
		const ResolutionPreset& oldResolution = dof.getBlurResolution();
		ResolutionPreset newResolution(GUISnippets::drawResolution(context, oldResolution));

		if (newResolution != oldResolution)
			dof.resizeBlurResolution(newResolution);

		float aperture = GUISnippets::drawBarFloatLogarithmic(context, dof.getAperture(), 0.f, 1000.f, 0.1f, "Aperture");
		dof.setAperture(aperture);

		float focal = GUISnippets::drawBarFloatLogarithmic(context, dof.getFocalLength(), 0.f, 1000.f, 0.1f, "Focal Length");
		dof.setFocalLength(focal);

		GUISnippets::drawTreeNode(context, "Blur", true, [this](GUIContext* context) {
			DepthOfFieldBlur& blur = dof.getBlur();

			float sigma = GUISnippets::drawBarFloatLogarithmic(context, blur.getSigma(), 0.1f, 25.f, 0.001f, "Sigma");
			blur.setSigma(sigma);

			float threshold = GUISnippets::drawBarFloat(context, blur.getThreshold(), 0.f, 1.f, 0.01f, "Threshold");
			blur.setThreshold(threshold);
		});
	}



	FXAASnippet::FXAASnippet(FXAA& fxaa) : PostEffectSnippet(fxaa), fxaa(fxaa) {}

	void FXAASnippet::drawSimple(GUIContext* context) {
		float blurMin = GUISnippets::drawBarFloat(context, fxaa.getBlurMin(), 0.f, 0.1f, 0.0001f, "Blur Min");
		fxaa.setBlurMin(blurMin);

		float fxaaMul = GUISnippets::drawBarFloat(context, fxaa.getFXAAMul(), 0.f, 0.5f, 0.0001f, "FXAA Factor");
		fxaa.setFXAAMul(fxaaMul);

		float fxaaMin = GUISnippets::drawBarFloat(context, fxaa.getFXAAMin(), 0.f, 0.001f, 0.0001f, "FXAA Min");
		fxaa.setFXAAMin(fxaaMin);

		float fxaaClamp = GUISnippets::drawBarFloat(context, fxaa.getFXAAClamp(), 1.f, 32.f, 0.1f, "FXAA Clamp");
		fxaa.setFXAAClamp(fxaaClamp);
	}



	GodRaySnippet::GodRaySnippet(GodRay& ray) : PostEffectSnippet(ray), ray(ray) {}

	void GodRaySnippet::drawSimple(GUIContext* context) {
		glm::vec3 position = GUISnippets::drawVector(context, ray.getLightPosition(), "", 100, 0.1f);
		ray.setLightPosition(position);

		int samples = GUISnippets::drawBarInteger(context, ray.getSampleCount(), 0, 200, 1, "Samples");
		ray.setSampleCount(samples);
	}

	

	LensFlareSnippet::LensFlareSnippet(LensFlare& flare) : PostEffectSnippet(flare), flare(flare) {}

	void LensFlareSnippet::drawSimple(GUIContext* context) {
		int active = !flare.getBloomUse();
		nk_checkbox_label(context, "Bloom", &active);
		flare.setBloomUse(!active);

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



	VolumetricLightSnippet::VolumetricLightSnippet(VolumetricLight& light) : PostEffectSnippet(light), light(light) {}

	void VolumetricLightSnippet::drawSimple(GUIContext* context) {
		int samples = GUISnippets::drawBarInteger(context, light.getSampleCount(), 0, 500, 1, "Samples");
		light.setSampleCount(samples);

		float density = GUISnippets::drawBarFloat(context, light.getDensity(), 0.f, 5.f, 0.001f, "Density");
		light.setDensity(density);

		float distance = GUISnippets::drawBarFloat(context, light.getMinDistance(), 0.f, 50.f, 0.1f, "Distance");
		light.setMinDistance(distance);
	}
	


	SSAOSnippet::SSAOSnippet(SSAO& ssao) : PostEffectSnippet(ssao), ssao(ssao) {}

	void SSAOSnippet::drawSimple(GUIContext* context) {
		float radius = GUISnippets::drawBarFloat(context, ssao.getRadius(), 0.1f, 100.f, 0.1f, "Radius");
		ssao.setRadius(radius);
	}



	SSRRSnippet::SSRRSnippet(SSRR& ssrr) : PostEffectSnippet(ssrr), ssrr(ssrr) {}

	void SSRRSnippet::drawSimple(GUIContext* context) {
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

	

	ConeTracerSnippet::ConeTracerSnippet(VoxelConeTracer& tracer) : PostEffectSnippet(tracer), tracer(tracer) {}

	void ConeTracerSnippet::drawSimple(GUIContext* context) {
		unsigned int specLOD = GUISnippets::drawBarInteger(context, tracer.getSpecularLOD(), 1, 10, 1, "Specular LOD");
		tracer.setSpecularLOD(specLOD);

		unsigned int specSteps = GUISnippets::drawBarInteger(context, tracer.getSpecularSampleSize(), 1, 100, 1, "Specular Size");
		tracer.setSpecularSampleSize(specSteps);

		unsigned int diffLOD = GUISnippets::drawBarInteger(context, tracer.getDiffuseLOD(), 1, 10, 1, "Diffuse LOD");
		tracer.setDiffuseLOD(diffLOD);

		unsigned int diffSteps = GUISnippets::drawBarInteger(context, tracer.getDiffuseSampleSize(), 1, 50, 1, "Diffuse Size");
		tracer.setDiffuseSampleSize(diffSteps);
	}



}