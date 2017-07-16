#include <glm.hpp>
#include "../transformation/transform.h"
#include "../cameras/camera.h"
#include "../texturing/imagetexture.h"
#include "gaussianblur.h"
#include "lensflare.h"

using namespace glm;

namespace geeL {

	LensFlare::LensFlare(BlurredPostEffect& filter, float scale, float samples, float resolution)
		: PostProcessingEffect("renderer/postprocessing/lensflare.frag"),
			filter(filter), resolution(resolution), strength(1.f), scale(scale), samples(samples) {}


	void LensFlare::setImageBuffer(const Texture& texture) {
		PostProcessingEffect::setImageBuffer(texture);

		filter.setImageBuffer(texture);
	}

	void LensFlare::init(ScreenQuad& screen, IFrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setFloat("scale", scale);
		shader.setFloat("samples", samples);
		shader.setFloat("strength", strength);

		filterBuffer.init(Resolution(parentBuffer->getResolution(), resolution),
			ColorType::RGB16, FilterMode::Linear, WrapMode::Repeat);
		filter.init(screen, filterBuffer);

		addImageBuffer(filterBuffer.getTexture(), "brightnessFilter");
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

			shader.use();
			shader.setFloat("strength", strength);
		}
	}

	void LensFlare::setScale(float value) {
		if (scale != value && value > 0.f) {
			scale = value;

			shader.use();
			shader.setFloat("scale", scale);
		}
	}

	void LensFlare::setMaxSamples(float value) {
		if (samples != value && value > 0.f) {
			samples = value;

			shader.use();
			shader.setFloat("samples", samples);
		}
	}

	void LensFlare::setDistortion(const glm::vec3& value) {
		if (distortion != value) {
			distortion = value;

			shader.use();
			shader.setVector3("distortion", distortion);
		}
	}

	void LensFlare::setStarburstTexture(const ImageTexture & texture) {
		shader.use();
		shader.setInteger("useStarburst", true);
		shader.addMap(texture, "starburst");
	}

	void LensFlare::setDirtTexture(const ImageTexture& texture) {
		shader.use();
		shader.setInteger("useDirt", true);
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
		filterBuffer.fill(filter);

		parentBuffer->resetSize();
		parentBuffer->bind();
		
		Transform& transform = camera->transform;
		vec3 camX = transform.getRightDirection(); 
		vec3 camZ = transform.getForwardDirection();
		float rot = dot(camX, vec3(0.f, 0.f, 1.f)) + dot(camZ, vec3(0.f, 1.f, 0.f));

		mat3 rotation = mat3(
			cos(rot), sin(rot), 0.f,
			-sin(rot), cos(rot), 0.f,
			0.f, 0.f, 1.0f
		);

		shader.use();
		shader.setMat3("starTransform", transform2 * rotation * transform1);
	}


}