#ifndef SSAO_H
#define SSAO_H

#include <vector>
#include <vec3.hpp>
#include "../texturing/imagetexture.h"
#include "../framebuffer/framebuffer.h"
#include "../utility/worldinformation.h"
#include "postprocessing.h"

namespace geeL {

	class BilateralFilter;

	//Screen Space Ambient Occlusion Post Effect
	class SSAO : public PostProcessingEffect, public WorldMapRequester, public CameraRequester {

	public:
		SSAO(PostProcessingEffect& blur, float radius = 5.f, Resolution resolution = HALFSCREEN);
		SSAO(const SSAO& other);
		~SSAO();

		SSAO& operator= (const SSAO& other);

		//Empty setter since base image isn't needed for SSAO

		virtual void setBuffer(unsigned int buffer) {}
		virtual void init(ScreenQuad& screen, const ColorBuffer& buffer);
		virtual void draw();

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		float getRadius() const;
		void setRadius(float radius);

		const Resolution& getResolution() const;

	protected:
		virtual void bindValues();

	private:
		bool ssaoPass = false;
		float radius;
		unsigned int sampleCount = 32;
		Resolution resolution;

		ImageTexture* noiseTexture;
		std::vector<glm::vec3> kernel;
		std::vector<glm::vec3> noise;
		PostProcessingEffect& blur;

		ShaderLocation projectionLocation;
		ColorBuffer tempBuffer;
		
	};
}

#endif