#include <glm.hpp>
#include "transformation/transform.h"
#include "cameras/camera.h"
#include "texturing/imagetexture.h"
#include "texturing/rendertexture.h"
#include "framebuffer/framebuffer.h"
#include "gaussianblur.h"
#include "lensflare.h"

using namespace glm;

namespace geeL {

	LensFlare::LensFlare(BlurredPostEffect& filter, 
		float scale, 
		float samples, 
		const ResolutionPreset& resolution)
			: PostProcessingEffectFS("shaders/postprocessing/lensflare.frag")
			, filter(filter)
			, filterResolution(resolution)
			, strength(1.f)
			, scale(scale)
			, samples(samples) {}

	LensFlare::~LensFlare() {
		if (filterTexture != nullptr) delete filterTexture;
	}


	void LensFlare::setImage(const Texture& texture) {
		PostProcessingEffectFS::setImage(texture);

		filter.setImage(texture);
	}

	void LensFlare::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		shader.bind<float>("scale", scale);
		shader.bind<float>("samples", samples);
		shader.bind<float>("strength", strength);

		Resolution filterRes = Resolution(parentBuffer->getResolution(), filterResolution);
		if (filterTexture == nullptr)
			filterTexture = new RenderTexture(filterRes, ColorType::RGB16,
				WrapMode::Repeat, FilterMode::Linear);
		else
			filterTexture->resize(filterRes);

		filter.init(PostProcessingParameter(parameter, filterRes));

		addTextureSampler(*filterTexture, "brightnessFilter");
	}

	float LensFlare::getStrength() const {
		return strength;
	}

	float LensFlare::getScale() const {
		return scale;
	}

	float LensFlare::getMaxSamples() const {
		return samples;
	}

	const glm::vec3& LensFlare::getDistortion() const {
		return distortion;
	}

	void LensFlare::setStrength(float value) {
		if (strength != value && value > 0.f) {
			strength = value;

			shader.bind<float>("strength", strength);
		}
	}

	void LensFlare::setScale(float value) {
		if (scale != value && value > 0.f) {
			scale = value;

			shader.bind<float>("scale", scale);
		}
	}

	void LensFlare::setMaxSamples(float value) {
		if (samples != value && value > 0.f) {
			samples = value;

			shader.bind<float>("samples", samples);
		}
	}

	void LensFlare::setDistortion(const glm::vec3& value) {
		if (distortion != value) {
			distortion = value;

			shader.bind<glm::vec3>("distortion", distortion);
		}
	}

	void LensFlare::setStarburstTexture(const ImageTexture & texture) {
		shader.bind<int>("useStarburst", true);
		shader.addMap(texture, "starburst");
	}

	void LensFlare::setDirtTexture(const ImageTexture& texture) {
		shader.bind<int>("useDirt", true);
		shader.addMap(texture, "dirt");
	}


	mat3 transform1 = mat3(
		2.f,  0.f,  0.f,
		0.f,  2.f,  0.f,
		-1.f, -1.f, 1.f
	);

	mat3 transform2 = mat3(
		0.5f, 0.f,  0.f,
		0.f,  0.5f, 0.f,
		0.5f, 0.5f, 1.f
	);

	void LensFlare::bindValues() {
		Transform& transform = camera->transform;
		vec3 camX = transform.getRightDirection(); 
		vec3 camZ = transform.getForwardDirection();
		float rot = dot(camX, vec3(0.f, 0.f, 1.f)) + dot(camZ, vec3(0.f, 1.f, 0.f));

		mat3 rotation = mat3(
			cos(rot), sin(rot), 0.f,
			-sin(rot), cos(rot), 0.f,
			0.f, 0.f, 1.0f
		);

		shader.bind<glm::mat3>("starTransform", transform2 * rotation * transform1);

		filter.bindValues();
	}

	void LensFlare::drawSubImages() {
		parentBuffer->add(*filterTexture);
		parentBuffer->fill(filter, clearColor);
	}


}